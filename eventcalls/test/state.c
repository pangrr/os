#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

struct prinfo
{
   long state;                      /* current state of process */
   pid_t pid;                       /* process id (input) */
   pid_t parent_pid;                /* process id of parent */
   pid_t youngest_child_pid;        /* process id of youngest child */
   pid_t younger_sibling_pid;       /* pid of the oldest among younger siblings */
   pid_t older_sibling_pid;         /* pid of the youngest among older siblings */
   struct timespec start_time;      /* process start time */
   unsigned long user_time;         /* CPU time spent in user mode */
   unsigned long sys_time;          /* CPU time spent in system mode */
   unsigned long cutime;            /* total user time of children */
   unsigned long cstime;            /* total system time of children */
   long uid;                        /* user id of process owner */
   char comm[16];                   /* name of program executed */
   unsigned long signal;            /* The set of pending signals */
};


int main (int argc, char **argv)
{
	if (argc != 2) { /* input arguments count wrong */
		printf("input error\n");
		return 0;
	}
		
	pid_t pid;
	pid = atoi(argv[1]);
	
	struct prinfo *p = (struct prinfo*) malloc(sizeof(struct prinfo));    
	p->pid = pid;
     
	if (syscall(299, p) != 0){
		printf("ERROR\n");
		exit(0);
	}

	printf("state: %d\n", p->state);
	return 0;
}
