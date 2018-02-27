all:
	gcc -Wall -g -o test tknz.c shell.c signalStuff.c cmdExec.c ll.c

clean:
	rm test
