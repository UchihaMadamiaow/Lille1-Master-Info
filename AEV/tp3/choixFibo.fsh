
variable read
variable print
variable fiboHard
variable fiboSoft

slave

begin

master

read function
	f lit
	waitbtn
	switch
	leddup
return

print function
	
    bufoutdup
return

fiboHard function
	switch
	leddup
	ticraz

	fibo

	tic 
	datapush
	print call		
	f lit
	waitbtn
	datapop
	print call
return

fiboSoft function
	0 lit 
	1 lit
	switch
	leddup	
	2 lit
	minus
	ticraz
	do
		tuck
		add
	loop
	tic 
	datapush
	print call		
	f lit
	waitbtn
	datapop
	print call
return

begin
	repeat
		f lit
		waitbtnpush		
		dup
		1 lit
		eq
		if
			fiboSoft call
		endif
		dup
		2 lit
		eq
		if
			fiboHard call
		endif
	again
endprogram

