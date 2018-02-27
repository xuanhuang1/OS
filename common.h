#ifndef COMMON_H
#define COMMON_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>



enum TF{FALSE,TRUE};

typedef struct job
{
  struct job *next;           /* next job */
  struct job *prev;           /* next job */
  char *argv;                /* for print */
  int jobid;                       /*   job ID   */
  pid_t pgid;                 /* process group ID, the same as the process ID for current process */
  int status;                 /* status */
  struct termios tmodes;      /* saved terminal modes */
} job;

extern struct job **first_job;
extern struct job **last_job;
extern char** args;
extern int argNum;
extern int* jobID;
extern pid_t shell_pgid;
extern struct termios shell_tmodes;
extern int shell_terminal;
extern int shell_is_interactive;


#endif