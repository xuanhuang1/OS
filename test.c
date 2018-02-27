#ifndef _REENTRANT
  #define _REENTRANT
#endif

#include "common.h"
#include "toknizer.h"
#include "ll.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>

#define PROGFAIL -1

enum cmds {EXIT, MAXBUILTIN};

const char* builtInCommands[MAXBUILTIN] = {"exit"};

int argNum;
char** args=NULL;
job **first_job =NULL, **last_job=NULL;
int* jobID;

int testTknz(){
  char string[] = "The sentence&; a aca ;g hd\0";

  int n = parse(string);

  for(int j=0;j<n;j++){
    printf("token b:%s \n",args[j]);
    free(args[j]);
  }

  free(args);


  return 1;
}

void initSharedVar(){
   first_job = mmap(NULL, sizeof(job*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   last_job = mmap(NULL, sizeof(job*), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   jobID = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
   *first_job = NULL;
   *last_job = NULL;
   *jobID = 0;

}

int testLl(){
	int status;
	initSharedVar();
	char  s[2] = "ab";
	char  s2[4] = "a2b2";
	//pid_t pid = fork();
	//if(pid == 0){
		push(s, getpid());
		printf("in childe\n");
	//}
	//wait(&status);
	//for (int i = 1; i < 10; i++)
	//	push(s2, getpid()+i);
	print_list();
	/*removeJob(first_job);
	print_list();
	removeJob(last_job);*/
	//job* j = findJobByPgid(getpid()+1);
	clearList();
	//removeJob(j);
	//job* j1 = findJobByjobID(0);
	//clearList();
	/*removeJob(j1);
	print_list();
	clearList();
	print_list();*/
}


int main(){
  testLl();
}
