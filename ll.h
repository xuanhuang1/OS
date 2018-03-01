#ifndef LL_H
#define LL_H

#include "common.h"

int push (int bgmode, char**, pid_t);
void print_list();
job* findJobByPgid(pid_t pgid);
job* findJobByjobID(int jid);
job* findLastSusJob();
int removeJob(job* j);
int isEmpty();
int clearList();
#endif
