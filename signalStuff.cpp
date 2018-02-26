
#include "common.h"
#include "signalStuff.h"

#include <signal.h>
#include <errno.h>

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
