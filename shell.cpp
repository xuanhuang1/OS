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
#include <termios.h>
#include <signal.h>
#include <errno.h>

#include "common.h"
#include "toknizer.h"

#define PROGFAIL -1

enum cmds {EXIT, MAXBUILTIN};

const char* builtInCommands[MAXBUILTIN] = {"exit"};

int argNum;
char** args=NULL;
struct job *first_job = NULL;

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

void checkExit(char** token, char* line);
int exeCmds(char**,char* line, int );
int initSigHd();

/* Make sure the shell is running interactively as the foreground job
   before proceeding. */

void init_shell (){

  /* See if we are running interactively.  */
  shell_terminal = STDIN_FILENO;
  shell_is_interactive = isatty (shell_terminal);

  if (shell_is_interactive)
    {
      while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
        kill (- shell_pgid, SIGTTIN);

      /* Put ourselves in our own process group.  */
      shell_pgid = getpid ();
      if (setpgid (shell_pgid, shell_pgid) < 0)
        {
          perror ("Couldn't put the shell in its own process group");
          exit (1);
        }

      /* Grab control of the terminal.  */
      tcsetpgrp (shell_terminal, shell_pgid);

      /* Save default terminal attributes for shell.  */
      tcgetattr (shell_terminal, &shell_tmodes);

    }
}


//this function is the signal handler                                                             
void handle_sigchld(int sig, siginfo_t *sip, void *notused) {
    if (sip->si_code == CLD_EXITED)
      printf ("Voluntary exit.\n");
    else  if (sip->si_code == CLD_STOPPED)
      printf ("Suspended.\n");
    else if ( (sip->si_code == CLD_KILLED) ||(sip->si_code == CLD_DUMPED) )     
      printf ("Croaked");

    else printf ("Nothing interesting\n");
    
    //}else
    //if no update, print not interested.                                                     
    //printf("Not interested pid:%d\n", getpgid(getpid()));
}
void sghd(int a){
  printf("a sig %d \n", a);
}

int main(int argc, char** argv){
  initSigHd();
  init_shell();
  while(1){
    size_t sizeInput;
    char* line = NULL;
    sigset_t sigset;

    printf("theShell: ");

    // block everything while read()
    sigemptyset(&sigset);
    for (int i = 0; i < SIGSYS+1; ++i)
      sigaddset(&sigset, i);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    int read = getline(&line, &sizeInput, stdin);
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);

    if( read == PROGFAIL){
      printf("Fail reading arguments!\n");
      fprintf(stderr, "Error : %s\n", strerror( errno ));
    }else{
      argNum = parse(line);
      
      if(argNum != 0){
        if(!args){printf("Fail mallocing command lines!\n");exit(0);}

        int lastIndx = 0; int bgmode = FALSE;
        for(int i=0;i<argNum;i++){
          if(strcmp(args[i],"&") == 0) bgmode = TRUE;

          if( (strcmp(args[i],";") == 0) || (strcmp(args[i],"&") == 0) || (i==argNum-1) ){

           if((i == argNum-1) && (strcmp(args[i],";")) && (strcmp(args[i],"&") )) i++;
           char** toks = (char**)malloc(sizeof(char*)*(i-lastIndx+1));
           for(int k=lastIndx; k<i;k++) toks[k-lastIndx] = args[k];
             toks[i-lastIndx] = NULL;
	    //for(int k=lastIndx; k<i;k++) printf("toks: %s",toks[k-lastIndx]);

           lastIndx = i+1;
           checkExit(toks, line);
           exeCmds(toks,line, bgmode);
           bgmode = FALSE;
           free(toks);
         }
       }
     }
     free(line);
     for(int j=0;j<argNum;j++)
       free(args[j]);
     free(args);
     argNum = 0;
   }
 }
}

int initSigHd(){
        //declare the sample sigaction struct                                                     
  struct sigaction sa;
        //set the handler, in this case our handler has three arguments, so will set sa_sigaction                                                                                              
        //instead of sa_handler                                                                   
  sa.sa_sigaction = &handle_sigchld;
        //initialize an empty set for signals to be blocked.                                      
  sigemptyset(&sa.sa_mask);
        //set flags, SA_SIGINFO flag is to specify signal handler in sa is sa_sigaction.          
  sa.sa_flags = SA_SIGINFO;
        //register the signal SIGCHLD.                                                            
  
  sigaction (SIGCHLD, &sa, NULL);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGINT, SIG_IGN);

  return TRUE;
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
  for(int i=index; (unsigned)i< strlen(string); i++){
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
    for(int j=0;j<argNum;j++)
      free(args[j]);
    free(args);
    exit(EXIT_SUCCESS);
  }
  setpgid(pid, pid);
  
  if(!bgmode){
    waitpid(pid, &status, WUNTRACED);
    //  printf("pgid parent: %d\n", getpgid(getpid()));
  }
  tcsetpgrp (shell_terminal, shell_pgid);
  tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);
  

  
  return 1;
  
}