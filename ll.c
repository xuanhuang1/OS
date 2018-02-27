
#include "common.h"
#include "ll.h"

int push (int jobid, char ** argv, int argLen , pid_t pid, int * jobID){
	job *current = malloc(sizeof(job));
	if(isEmpty()) *jobID = 0;
	current->pgid = pid;
	current->jobid = *jobID;
	current->status = 1;
	current->next = NULL;
	current->prev = NULL;

	(*jobID) += 1;
	char** strings = (char**)malloc(sizeof(char*)*(argLen+1));
	for (int i = 0; i < argLen; ++i){
		strings[i] = malloc(strlen(argv[i]) + 1);
		strcpy(strings[i], argv[i]);
		strings[i][strlen(argv[i])] = '\0';
	}
  	strings[argLen] = NULL;
  	current->argv = strings;

	if(!first_job){
		first_job = current;
		last_job = current;
		return TRUE;
	}
	current->prev = NULL;
	current->next = first_job;
	first_job = current;
	first_job->next->prev = first_job;
	return TRUE;
}

void print_list() {
    job * current = first_job;

	//printf("in order: \n");
    while (current != NULL) {
        printf("jobid[%d]: %5d %5d\n", current->jobid, current->pgid, current->status);
        current = current->next;
    }
    printf("done.\n");
/*
	printf("reverse: \n");
    current = last_job;
    while (current != NULL) {
        printf("jobid[%d]: %5d %5d\n", current->jobid, current->pgid, current->status);
        current = current->prev;
    }
    printf("done.\n");*/
}

job* findJobByPgid(pid_t pgid){
	job *current = first_job;
	while(current->next){
		if(current->pgid == pgid)
			return current;
		current = current->next;
	}
	return NULL;
}

job* findJobByjobID(int jid){
	job *current = first_job;
	while(current->next){
		if(current->jobid == jid)
			return current;
		current = current->next;
	}
	return NULL;
}

int removeJob(job* j){
	job * temp = j->prev;
	if(!temp){
		if(!j->next) last_job = NULL;
		else j->next->prev = NULL;
		first_job = j->next;
		j->next = NULL;
	}else if(!j->next){
		if(!temp) first_job = NULL;
		else j->prev->next = NULL;
		last_job = j->prev;
		j->prev = NULL;
	}else{
		temp->next = j->next;
		(j->next)->prev = temp; 
	}
	int i=0;
	while(j->argv[i] != NULL){
		free(j->argv[i]);
		i++;
	}
	free(j->argv);
	free(j);
	return TRUE;
}

int isEmpty(){
	if(!first_job && !last_job) return TRUE;
	if(!first_job){printf("Last job still alive! shouldn't happen!\n");}
	if(!last_job){printf("First job still alive! shouldn't happen!\n");}
	return FALSE;

}

int clearList(){
	job* j = first_job;
	while(j){
		job* temp = j->next;
		removeJob(j);
		j = temp;
	}
	return TRUE;

}