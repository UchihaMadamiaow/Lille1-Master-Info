
variable read
variable print

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

begin
	repeat
		0 lit 
		1 lit
		read call
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
	again
endprogram
