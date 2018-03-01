
#include "common.h"
#include "signalStuff.h"

#include <signal.h>
#include <errno.h>
#include "ll.h"

//this function is the signal handler                                                             
void handle_sigchld(int sig, siginfo_t *sip, void *notused) {
    if (sip->si_code == CLD_EXITED){
      job *j = findJobByPgid(sip->si_pid);
      printf ("\nVoluntary exit job[%d] %d %s.\n", j->jobid, sip->si_pid, j->argv);
      removeJob(j);
    }else  if (sip->si_code == CLD_STOPPED){
      job *j = findJobByPgid(sip->si_pid);  
      printf ("\nSuspended job[%d] %d %s.\n", j->jobid, sip->si_pid, j->argv);
      tcgetattr (shell_terminal, &j->tmodes);
      j->status = STOPPED;

    }else if ( (sip->si_code == CLD_KILLED) ||(sip->si_code == CLD_DUMPED) ) {
      job *j = findJobByPgid(sip->si_pid);   
      printf ("\nkilled job[%d] %d %s.\n", j->jobid, sip->si_pid, j->argv);
      removeJob(j);

    }else if(sip->si_code == CLD_CONTINUED){
      job *j = findJobByPgid(sip->si_pid);
      j->status = RUNNING;
    }else printf ("Nothing interesting\n");
    
    //}else
    //if no update, print not interested.                                                     
    //printf("Not interested pid:%d\n", getpgid(getpid()));
}


int initSigHd(){
        //declare the sample sigaction struct                                                     
  struct sigaction sa;
        //set the handler, in this case our handler has three arguments, so will set sa_sigaction                                                                                              
        //instead of sa_handler                                                                   
  sa.sa_sigaction = &handle_sigchld;
        //initialize an empty set for signals to be blocked.                                      
  sigfillset(&sa.sa_mask);
        //set flags, SA_SIGINFO flag is to specify signal handler in sa is sa_sigaction.          
  sa.sa_flags = SA_RESTART | SA_SIGINFO;
        //register the signal SIGCHLD.                                                            
  
  if(sigaction (SIGCHLD, &sa, NULL)==-1)perror("Error handling SIGCHLD");;
  signal(SIGTTOU, SIG_IGN);
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGQUIT, SIG_IGN); 


  return TRUE;
}
