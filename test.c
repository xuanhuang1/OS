#ifndef _REENTRANT
  #define _REENTRANT
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"
#include "toknizer.h"
#include "ll.h"

#define PROGFAIL -1

enum cmds {EXIT, MAXBUILTIN};

const char* builtInCommands[MAXBUILTIN] = {"exit"};

int argNum;
char** args=NULL;
job *first_job, *last_job;

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

int testLl(){
	int jobID = 0;
	char * s[2] = {"a","b"};
	char * s2[2] = {"a2","b2"};
	push(jobID, s,2, getpid(), &jobID);
	for (int i = 1; i < 10; i++)
		push(jobID, s2,2, getpid()+i, &jobID);
	print_list();
	/*removeJob(first_job);
	print_list();
	removeJob(last_job);*/
	job* j = findJobByPgid(getpid()+1);
	//clearList();
	removeJob(j);
	job* j1 = findJobByjobID(3);
	//clearList();
	removeJob(j1);
	print_list();
	clearList();
	print_list();
}


int main(){
  testLl();
}
