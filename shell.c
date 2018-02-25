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

#define PROGFAIL -1

enum cmds {EXIT, MAXBUILTIN};

const char* builtInCommands[MAXBUILTIN] = {"exit"};

int argNum;
char** args=NULL;
struct job *first_job = NULL;

void checkExit(char** token, char* line);
int exeCmds(char**,char* line, int );
int main(int argc, char** argv){
  

  while(1){
    size_t sizeInput;
    char* line = NULL;

    printf("theShell: ");
    if(getline(&line, &sizeInput, stdin) == PROGFAIL){
      printf("Fail reading arguments!\n");
    }else{
      argNum = parse(line);
      
      if(argNum != 0){
        if(!args){printf("Fail mallocing command lines!\n");exit(0);}

	int lastIndx = 0;
	for(int i=0;i<argNum;i++){
	  if(strcmp(args[i],";") == 0 || (i==argNum-1)){
	    if(i == argNum-1) i++;
	    char** toks = (char**)malloc(sizeof(char*)*(i-lastIndx+1));
	    for(int k=lastIndx; k<i;k++) toks[k-lastIndx] = args[k];
	    toks[i-lastIndx] = NULL;
	    //for(int k=lastIndx; k<i;k++) printf("toks: %s",toks[k-lastIndx]);
	    
	    lastIndx = i+1;
	    checkExit(toks, line);
	    exeCmds(toks,line, 0);
	    free(toks);
	  }
	}
      }
      free(line);
      for(int j=0;j<argNum;j++)
	//printf("aaaaaa num %d\n",j);
	free(args[j]);
      free(args);
      argNum = 0;
    }
  }
}


void checkExit(char** toks, char* line){
  if(!toks[0]) return;
  if(strcmp(toks[0], builtInCommands[EXIT])== 0){
    //printf("exit shell ||%s||\n",token);
    free(line);
    free(toks);
    for(int j=0;j<argNum;j++)
      free(args[j]);
    free(args);
    exit(EXIT_SUCCESS);
  }
}

int allDigitFrom(int index, char* string){
  int num = 0;
  for(int i=index; i< strlen(string); i++){
    if(!isdigit(string[i]) || string[i] == '-'){
      return -1;
    }else
      num = num*10 + (string[i]-'0');
  }
  return num;
}

int exeCmds(char** toks,  char* line, int bgmode){

  if(!toks){printf("Parsing failed in exeCmd\n");return 1;}
  if(!toks[0]){printf("No valid args input for program!\n");return 1;}
  
  // execute input program
  pid_t pid = 1;
  int status;
  pid = fork();
  if(pid == 0){
    //setpgid(0, 0);
    execvp(toks[0], &toks[0]);
    printf("\"%s\": command not found.\n",toks[0]);
    free(line);
    //for(int j=0;j<tokLen;j++)
    free(toks);
    for(int j=0;j<argNum;j++)
      free(args[j]);
    free(args);
    exit(EXIT_SUCCESS);
  }

  if(!bgmode)
    wait(&status);

  return 1;
  
}
