
#include "common.h"
#include "cmdExec.h"
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

enum cmds {EXIT, JOBS, BG, FG,KILL, MAXBUILTIN};

const char* builtInCommands[MAXBUILTIN] = {"exit", "jobs", "bg", "fg","kill"};

void freeGlobals(){
  for(int j=0;j<argNum;j++)
    free(args[j]);
  free(args);
  //clearList();
}

int allDigitFrom(int index, char* string){
  int num = 0;
  for(int i=index; (unsigned)i< strlen(string); i++){

    if(!isdigit(string[i]) || string[i] == '-'){
      return -1;
    }else
      num = num*10 + (string[i]-'0');
  }
  return num;
}

int checkBuiltIn(char** toks, char* line){
  if(!toks[0]) return FALSE;

  if(strcmp(toks[0], builtInCommands[EXIT])== 0){
    //printf("exit shell ||%s||\n",token);
    free(toks);
    freeGlobals();
    clearList();
    free(line);
    exit(EXIT_SUCCESS);
      

  }else if(strcmp(toks[0], builtInCommands[JOBS])== 0){
    print_list();
    return TRUE;
  }else if(strcmp(toks[0], builtInCommands[BG])== 0){
    if( toks[1]){
      if ((toks[1][0] == '%') && (allDigitFrom(1, (toks[1])+0) != -1) ){
        int n = allDigitFrom(1, toks[1]+0);
        if(n == -1){printf("not a valid num after bg \n");return TRUE;}
        printf("read bg with num :%d\n", n);
        //kill(SIGCONT, );

        return TRUE;
      }
      printf("wrong format bg \n");return TRUE;
    }else{
      printf("do bg\n");return TRUE;
    }
    return TRUE;
  }else if(strcmp(toks[0], builtInCommands[FG])== 0){
    if( toks[1]){
      if ((toks[1][0] == '%') && (allDigitFrom(1, (toks[1])+0) != -1) ){
        int n = allDigitFrom(1, toks[1]+0);
        if(n == -1){printf("not a valid num after fg \n");return TRUE;}
        printf("read fg with num :%d\n", n);

        return TRUE;
      }
      printf("wrong format fg \n");return TRUE;
    }else{
      printf("do fg\n");return TRUE;
    }
    return TRUE;
  }else if(strcmp(toks[0], builtInCommands[KILL])== 0){
    if( toks[1]){
      if ((toks[1][0] == '%') && (allDigitFrom(1, (toks[1])+0) != -1) ){
        int n = allDigitFrom(1, toks[1]+0);
        if(n == -1){printf("not a valid num after kill \n");return TRUE;}
        printf("read kill with num :%d\n", n);

        return TRUE;
      }
      printf("wrong format kill \n");return TRUE;
    }else{
      printf("do kill\n");return TRUE;
    }
    return TRUE;
  }else{}

  return FALSE;
}

int exeCmds(char** toks,  char* line, int bgmode){

  if(!toks){printf("Parsing failed in exeCmd\n");return 1;}
  if(!toks[0]){printf("No valid args input for program!\n");return 1;}
  
  // execute input program
  pid_t pid;
  int status;
  
  pid = fork();
  if(pid == 0){
    setpgid(getpid(), getpid());
    //printf("pgid child: %d \n", getpgid(getpid()));
    if(!bgmode)
      tcsetpgrp (shell_terminal, getpid());

    signal(SIGTTOU, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);

    execvp(toks[0], &toks[0]);

    printf("\"%s\": command not found.\n",toks[0]);
    free(line);
    //for(int j=0;j<tokLen;j++)
    free(toks);
    freeGlobals();
    exit(EXIT_SUCCESS);
  }

  setpgid(pid, pid);
  push(line, pid);

  if(!bgmode)
    waitpid(pid, &status, WUNTRACED);


  tcsetpgrp (shell_terminal, shell_pgid);
  tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);
  

  
  return 1;
  
}

