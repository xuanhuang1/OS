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

int main(){
  char string[] = "The sentence&; a aca ;g hd\0";

  int n = parse(string);

  for(int j=0;j<n;j++){
    printf("token b:%s \n",args[j]);
    free(args[j]);
  }

  free(args);


  return 1;
}
