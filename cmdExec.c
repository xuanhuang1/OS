
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
#include <signal.h>

enum cmds {EXIT, JOBS, BG, FG,KILL, MAXBUILTIN};

const char* builtInCommands[MAXBUILTIN] = {"exit", "jobs", "bg", "fg","kill"};

void bgJob(job *j){
  if(j->status == STOPPED){
    if(kill (- j->pgid, SIGCONT)<0)
      perror("kill (SIGCONT)");
  }
}

void fgJob(job *j){
  tcsetpgrp(shell_terminal, j->pgid);

  int status;
  if(j->status == STOPPED){

    tcsetattr(shell_terminal,TCSADRAIN, &j->tmodes);

    if(kill (- j->pgid, SIGCONT)<0)
      perror("kill (SIGCONT)");
  }
  waitpid(j->pgid, &status, WUNTRACED);

  tcsetpgrp(shell_terminal, shell_pgid);
  tcsetattr(shell_terminal, TCSADRAIN, &shell_tmodes);

}

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
        if(n == -1){printf("not a valid num after bg %% \n");return TRUE;}
        printf("read bg with num :%d\n", n);
        job *j = findJobByjobID(n);
        if(j)
          bgJob(j);
        return TRUE;
      }
      printf("wrong format bg \n");return TRUE;
    }else{
      job *j = findLastSusJob();
      if(j)
        bgJob(j);
      printf("do bg\n");return TRUE;
    }
    return TRUE;
  }else if(strcmp(toks[0], builtInCommands[FG])== 0){
    if( toks[1]){
      if ((toks[1][0] == '%') && (allDigitFrom(1, (toks[1])+0) != -1) ){
        int n = allDigitFrom(1, toks[1]+0);
        if(n == -1){printf("not a valid num after fg %% \n");return TRUE;}
        printf("read fg with num :%d\n", n);
        job *j = findJobByjobID(n);
        if(j)
          fgJob(j);
        return TRUE;
      }
      printf("wrong format fg \n");return TRUE;
    }else{
      job *j = last_job;
        if(j)
          fgJob(j);
        return TRUE;
      printf("do fg\n");return TRUE;
    }
    return TRUE;
  }else if(strcmp(toks[0], builtInCommands[KILL])== 0){
    if( toks[1]){
      if ((toks[1][0] == '%') && (allDigitFrom(1, (toks[1])+0) != -1) ){
        int n = allDigitFrom(1, toks[1]+0);
        if(n == -1){printf("not a valid num after kill %% \n");return TRUE;}
        printf("read kill with num :%d\n", n);
        job *j = findJobByjobID(n);
        if(j){
          kill (- j->pgid, SIGCONT);
          kill(- j->pgid, SIGTERM);
        }
        return TRUE;
      }else if(strcmp(toks[1],"-9")==0){
        if ((toks[2][0] == '%') && (allDigitFrom(1, (toks[2])+0) != -1) ){
          int n = allDigitFrom(1, toks[1]+0);
          if(n == -1){printf("not a valid num after kill -9 %% \n");return TRUE;}
          printf("read kill -9 with num :%d\n", n);
          job *j = findJobByjobID(n);
          if(j){
            kill (- j->pgid, SIGCONT);
            kill(- j->pgid, SIGKILL);
          }
          return TRUE;
        }

      }
      printf("wrong format kill \n");return TRUE;
    }else{
      printf("use kill [-9] %%[number] \n");return TRUE;
    }
    return TRUE;
  }else{}

  return FALSE;
}

int exeCmds(char** toks,  char* line, int bgmode){

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGCHLD);

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
    signal(SIGTERM, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    

    execvp(toks[0], &toks[0]);

    printf("\"%s\": command not found.\n",toks[0]);
    free(line);

    free(toks);
    freeGlobals();
    exit(EXIT_SUCCESS);
  }

  setpgid(pid, pid);

  sigprocmask(SIG_BLOCK, &sigset, NULL);
  push(bgmode, toks, pid);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);



  if(!bgmode)
    waitpid(pid, &status, WUNTRACED);


  tcsetpgrp (shell_terminal, shell_pgid);
  tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);
  
  
  return 1;
  
}

