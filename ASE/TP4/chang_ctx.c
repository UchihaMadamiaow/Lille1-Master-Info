/*Author:
  DOUAIILE Erwan
  MIRANDA Yoan
*/
#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include "hw.h"

#define CTX_MAGIC 0xABCD
#define MAXCOUNT 100000000
#define N 100                         /* nombre de places dans le tampon */

struct ctx_s ctx_ping; 
struct ctx_s ctx_pong; 
struct ctx_s *ctx_cur; 

typedef void (func_t)(void *);

enum ctx_state_e{CTX_INIT,CTX_EXQ,CTX_BLK_SEM,CTX_END};

struct ctx_s{
  unsigned char *ctx_base;
  unsigned char *ctx_esp;
  unsigned char *ctx_ebp;
  unsigned ctx_size;
  func_t* ctx_f;
  void* ctx_arg;
  enum ctx_state_e ctx_state;
  unsigned ctx_magic;
  struct ctx_s * next;
  struct ctx_s * ctx_sem_next_ctx;
};

struct sem_s{
	int sem_count;
	struct ctx_s * sem_first_ctx;
	unsigned sem_magic;
};

static struct ctx_s * ring = (struct ctx_s*) 0; 	
static struct sem_s mutex, vide, plein;

static int objetP,objetC;


int create_ctx(int stack_size, func_t f, void* args){
  struct ctx_s *ctx = (void*)malloc(sizeof(struct ctx_s));
  if(!ring){
    ring = ctx;
    ctx->next = ctx;
  } 
  else{
    ctx->next = ring->next;
    ring->next = ctx;
  }
  return  init_ctx(ctx,stack_size, f, args);    
}


int init_ctx(struct ctx_s *ctx,int stack_size, func_t f, void* args){
  ctx->ctx_f=f;
  ctx->ctx_arg=args;
  ctx->ctx_state=CTX_INIT;
  ctx->ctx_base=(void*)malloc(stack_size);
  ctx->ctx_size=stack_size;
  ctx->ctx_esp=ctx->ctx_base+stack_size-4;
  ctx->ctx_ebp=ctx->ctx_base+stack_size-4;
  ctx->ctx_magic=0xABCD;
  return 0;
}

void start_current_ctx(){
  ctx_cur->ctx_state=CTX_EXQ;
  ctx_cur->ctx_f(ctx_cur->ctx_arg);
  ctx_cur->ctx_state=CTX_END;
}

void switch_to_ctx(struct ctx_s *ctx){
  assert(ctx);
  assert(ctx->ctx_magic==CTX_MAGIC);
  assert(ctx->ctx_state==CTX_INIT||ctx->ctx_state==CTX_EXQ || ctx->ctx_state==CTX_BLK_SEM );

  while( ctx->ctx_state == CTX_END || ctx->ctx_state==CTX_BLK_SEM) {
	if(ctx->ctx_state == CTX_END){
	    	ctx_cur->next = ctx->next;
	    	free(ctx->ctx_base);
	    	free(ctx);
	    	ctx = ctx_cur->next;
	}
	if(ctx->ctx_state==CTX_BLK_SEM){
		printf("bloque %d\n", ctx->ctx_ebp);
		ctx=ctx->next;
	}
  }
  irq_disable();
  if(ctx_cur){
    asm("movl %%esp,%0" "\n\t" "movl %%ebp,%1"
	:"=r"(ctx_cur->ctx_esp),"=r"(ctx_cur->ctx_ebp));
  }
  ctx_cur=ctx;
  irq_enable();
  asm("movl %0,%%esp" "\n\t" "movl %1,%%ebp"
      :
      :"r"(ctx->ctx_esp),"r"(ctx->ctx_ebp));
  if(ctx_cur->ctx_state==CTX_INIT)
    start_current_ctx();
	
}

void yield(){
  if(ctx_cur!=NULL)
    switch_to_ctx(ctx_cur->next); 
  else if (ring!=NULL)
    switch_to_ctx(ring->next);
  else 
    return;
}

void start_schedule(){

  setup_irq(TIMER_IRQ,yield);
  start_hw();  
  yield();
} 

void sem_init(struct sem_s * sem,unsigned int val){
	sem->sem_count=val;
	sem->sem_first_ctx=(struct ctx_s *)0;
	sem->sem_magic=0xABCD;
}

void sem_down(struct sem_s *sem){
	irq_disable();
	assert(sem->sem_magic==CTX_MAGIC);
	sem->sem_count--;
	if(sem->sem_count<0){
		ctx_cur->ctx_state=CTX_BLK_SEM;
		ctx_cur->ctx_sem_next_ctx=sem->sem_first_ctx;
		sem->sem_first_ctx=ctx_cur;
		irq_enable();
		yield();
	}else{
		irq_enable();
	}
}

void sem_up(struct sem_s * sem){
	irq_disable();
	assert(sem->sem_magic==CTX_MAGIC);
	sem->sem_count++;
	if(sem->sem_count<1){
		sem->sem_first_ctx->ctx_state=CTX_EXQ;
		sem->sem_first_ctx=sem->sem_first_ctx->ctx_sem_next_ctx;	
	}
	irq_enable();
}

void f_ping(void *args)
{
  int i;
  while(1) {
    printf("A\n") ;
    for(i=0;i<MAXCOUNT;i++)
      ;
    printf("B\n") ;
    for(i=0;i<MAXCOUNT;i++)
      ;
    printf("C\n") ;
  }
}

void f_pong(void *args)
{
  int i;
  while(1) {
    for(i=0;i<MAXCOUNT;i++)
      ;
    printf("1\n") ;
    for(i=0;i<MAXCOUNT;i++)
      ;
    printf("2\n") ;
  }
} 


void producteur (void *args)
{
  int i;
  objetP=0;
  while (1) {
        for(i=0;i<MAXCOUNT;i++)
      ;  
    objetP++;
    printf("debut production %d \n",objetP);
    sem_down(&vide);                  /* dec. nb places libres */
    sem_down(&mutex); 
    for(i=0;i<MAXCOUNT;i++)
      ;                /* entree en section critique */
    printf("fin production  %d \n",objetP);
    sem_up(&mutex);                   /* sortie de section critique */
    sem_up(&plein);                   /* inc. nb place occupees */
  }
}

void consommateur (void *args)
{
  int i;
  objetC=0;
  while (1) {
	    for(i=0;i<MAXCOUNT;i++)
      ;  
    objetC++;
    printf("debut consommateur %d \n",objetC);
    sem_down(&plein);                 /* dec. nb emplacements occupes */
    sem_down(&mutex);                 /* entree section critique */
    for(i=0;i<MAXCOUNT;i++)
      ;
    printf("fin consommateur  %d \n",objetC);    
    sem_up(&mutex);                   /* sortie de la section critique */
    sem_up(&vide);                    /* inc. nb emplacements libres */

  }
}

int main(int argc, char *argv[])
{

	sem_init(&mutex, 1);                /* controle d'acces au tampon */
	sem_init(&vide, N);                 /* nb de places libres */
	sem_init(&plein, 0);                /* nb de places occupees */
	create_ctx(16384, consommateur, NULL);
	create_ctx( 16384, producteur, NULL);
  
  start_schedule();

  exit(EXIT_SUCCESS);
}



