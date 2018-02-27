
#include "common.h"
#include "ll.h"
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>


int push (char * argv, pid_t pid){
	job *current = (job*)malloc(sizeof(job));

	if(isEmpty()) jobID = 0;
	current->pgid = pid;
	current->jobid = jobID;
	current->status = RUNNING;
	current->next = NULL;
	current->prev = NULL;
	tcgetattr (shell_terminal, &current->tmodes);

	jobID += 1;
	char* string = (char*)malloc(sizeof(char)*(strlen(argv)+1));
	memcpy(string, argv, strlen(argv));
  	string[strlen(argv)] = '\0';
  	//string = "aaa\0";
  	current->argv = string;

  	printf("push: %s\n", current->argv);
	if(!(first_job)){
		first_job = current;
		last_job = current;
		return TRUE;
	}
	current->prev = NULL;
	current->next = first_job;
	first_job = current;
	(first_job)->next->prev = (first_job);
	return TRUE;
}

void print_list() {
    job * current = first_job;

	printf("\n\nPrinting in reversed order: !!!!!\n");
    /*while (current != NULL) {
        printf("jobid[%d]: %10d %10s %10s", current->jobid, current->pgid, current->status?"Running":"stopped", current->argv);
        current = current->next;
    }
    printf("done.\n\n\n");*/

	//printf("reverse: \n");
    current = last_job;
    while (current != NULL) {
        printf("jobid[%d]: %10d %10s %10s", current->jobid, current->pgid, current->status?"Running":"stopped", current->argv);
        current = current->prev;
    }
    printf("done.\n\n\n");
}

job* findJobByPgid(pid_t pgid){
	job *current = first_job;
	if(!current) return NULL;
	if(current->pgid == pgid) return current;

	while(current->next){
		if(current->pgid == pgid)
			return current;
		current = current->next;
	}
	return NULL;
}

job* findJobByjobID(int jid){
	job *current = first_job;
	if(!current) return NULL;
	if(current->jobid == jid) return current;

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
	free(j->argv);
	free(j);
	j = NULL;	
	return TRUE;
}

int isEmpty(){
	if((first_job)== NULL && (last_job)==NULL) return TRUE;
	if((first_job)==NULL){printf("Last job still alive! shouldn't happen!\n");}
	if((last_job)==NULL){printf("First job still alive! shouldn't happen!\n");}
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