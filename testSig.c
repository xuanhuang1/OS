#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#define NEG -1

int main(){
int suc = fork();
	//check whether fork is successful.
	if (suc == -1){
		printf("Fork failure!\n");
		exit(EXIT_FAILURE);
	}
	if (suc == 0){
		//if in child process, exit with success.
		exit(EXIT_SUCCESS);
	}
	else{
		int i;
		// let the parent process live for five seconds, and see whether there are updates from child process at each second
		for (i = 0; i < 5; i++){
			printf("%d second(s) have passed, let's see whether there is update from child.\n", i);
			sleep(1);
		}
	}
	exit(EXIT_SUCCESS);
}
