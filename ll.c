
#include "common.h"
#include "ll.h"
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

int push (char * argv, pid_t pid){
	job *current = mmap(NULL, sizeof(job), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if(isEmpty()) (*jobID )= 0;
	current->pgid = pid;
	current->jobid = *jobID;
	current->status = 1;
	current->next = NULL;
	current->prev = NULL;

	(*jobID) += 1;
	char* string = mmap(NULL, sizeof(char)*(strlen(argv)+1), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	//(char*)malloc(sizeof(char)*(strlen(argv)+1));
	memcpy(string, argv, strlen(argv));
  	string[strlen(argv)] = '\0';
  	//string = "aaa\0";
  	current->argv = string;

  	printf("push: %s\n", current->argv);
	if(!(*first_job)){
		*first_job = current;
		*last_job = current;
		return TRUE;
	}
	current->prev = NULL;
	current->next = *first_job;
	*first_job = current;
	(*first_job)->next->prev = (*first_job);
	return TRUE;
}

void print_list() {
    job * current = *first_job;

	//printf("in order: \n");
    while (current != NULL) {
        printf("jobid[%d]: %5d %5d %5s\n", current->jobid, current->pgid, current->status, current->argv);
        current = current->next;
    }
    printf("done.\n");
/*
	printf("reverse: \n");
    current = *last_job;
    while (current != NULL) {
        printf("jobid[%d]: %5d %5d\n", current->jobid, current->pgid, current->status);
        current = current->prev;
    }
    printf("done.\n");*/
}

job* findJobByPgid(pid_t pgid){
	job *current = *first_job;
	if(!current) return NULL;

	while(current->next){
		if(current->pgid == pgid)
			return current;
		current = current->next;
	}
	return NULL;
}

job* findJobByjobID(int jid){
	job *current = *first_job;
	if(!current) return NULL;
	while(current->next){
		if(current->jobid == jid)
			return current;
		current = current->next;
	}
	return NULL;
}

int removeJob(job* j){
	if(!j) return FALSE;
	job * temp = j->prev;
	//printf("removeJob: %d\n", j->jobid);
	if(!temp){
		if(!j->next) *last_job = NULL;
		else j->next->prev = NULL;
		*first_job = j->next;
		j->next = NULL;
	}else if(!j->next){
		if(!temp) *first_job = NULL;
		else j->prev->next = NULL;
		*last_job = j->prev;
		j->prev = NULL;
	}else{
		temp->next = j->next;
		(j->next)->prev = temp; 
	}
    munmap(j->argv, strlen(j->argv));
    munmap(j, sizeof(job));
	//free(j->argv);
	//free(j);
	return TRUE;
}

int isEmpty(){
	if((*first_job)== NULL && (*last_job)==NULL) return TRUE;
	if((*first_job)==NULL){printf("Last job still alive! shouldn't happen!\n");}
	if((*last_job)==NULL){printf("First job still alive! shouldn't happen!\n");}
	return FALSE;

}

int clearList(){
	job* j = *first_job;
	while(j){
		job* temp = j->next;
		removeJob(j);
		j = temp;
	}
	return TRUE;

}