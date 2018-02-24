#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>

#include <termios.h>
#include <stdlib.h>


enum TF{FALSE,TRUE};

typedef struct job
{
  struct job *next;           /* next job */
  char **argv;                /* for exec */
  int jobid;                       /*   job ID   */
  pid_t pgid;                 /* process group ID, the same as the process ID for current process */
  int status;                 /* status */
  struct termios tmodes;      /* saved terminal modes */
} job;

extern struct job *first_job;
extern char** args;




#endif