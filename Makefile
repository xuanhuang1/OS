all:
	g++ -Wall -g -o test tknz.cpp shell.cpp signalStuff.cpp cmdExec.cpp

clean:
	rm test
