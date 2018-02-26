#ifndef _REENTRANT
  #define _REENTRANT
#endif

#include "common.h"
#include "signalStuff.h"
#include "toknizer.h"
#include "cmdExec.h"

#define PROGFAIL -1


int argNum;
char** args=NULL;
struct job *first_job = NULL;

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;



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

