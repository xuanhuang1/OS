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

void checkExit(char* token, char* line);
int exeCmds(char* line, int );
int test(){
  char string[] = "The sentence&; a aca ;g hd\0";

  int n = parse(string);

  for(int j=0;j<n;j++){
    printf("token b:%s \n",args[j]);
    free(args[j]);
  }

  free(args);


  return 1;

}
int main(int argc, char** argv){
  test();
  
/*
  while(1){
    size_t sizeInput;
    char* line = NULL;

    printf("theShell: ");
    if(getline(&line, &sizeInput, stdin) == PROGFAIL){
      printf("Fail reading arguments!\n");
    }else{
      argNum = parse(line);
      for (int i = 0; i < argNum; ++i)
      {
        printf("%s ", args[i]);
      }
        printf("\n");
      if(argNum != 0){
        if(!args){printf("Fail mallocing command lines!\n");exit(0);}
      
        checkExit(args[0], line);
        exeCmds(line, 0);

        argNum = 0;
      }
      free(line);
      for(int j=0;j<argNum;j++)
        free(args[j]);
      free(args);
    }
  }*/
}


void checkExit(char* token, char* line){
  if(strcmp(token, builtInCommands[EXIT])== 0){
    //printf("exit shell ||%s||\n",token);
    free(line);
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

int exeCmds(char* line, int bgmode){
  
  if(!args[0]){printf("No valid args!\n");return 1;}
  
  // execute input program
  pid_t pid = 1;
  int status;
  pid = fork();
  if(pid == 0){
    //setpgid(0, 0);
    execvp(args[0], &args[0]);
    printf("\"%s\": command not found.\n",args[0]);
    free(line);
    for(int j=0;j<argNum;j++)
      free(args[j]);
    free(args);
    exit(EXIT_SUCCESS);
  }

  //if(!bgmode)
    wait(&status);

  return 1;
  
}