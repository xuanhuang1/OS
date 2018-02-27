#ifndef LL_H
#define LL_H

#include "common.h"

int push (int, char **, int , pid_t, int*);
void print_list();
job* findJobByPgid(pid_t pgid);
job* findJobByjobID(int jid);
int removeJob(job* j);
int isEmpty();
int clearList();
#endif
