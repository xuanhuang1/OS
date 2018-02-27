all:
	gcc -Wall -g -lrt -o test tknz.c shell.c signalStuff.c cmdExec.c ll.c

clean:
	rm test vgcore.*
