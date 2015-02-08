/* 
 * expc.c
 * This program prints out its process id and blocks the SIGUSR2 signal.
 * Then is sends SIGUSR2 to itself twice and suspends itself. 
 */ 


#include <linux/unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

struct prinfo {
        long state;                             /* current state of process */
        pid_t pid;                              /* process id (input) */
        pid_t parent_pid;                       /* process id of parent which receives SIGCHLD */
        //pid_t real_parent_pid;                   /* process id of real parent */
        pid_t youngest_child_pid;               /* process id of youngest child */
        pid_t younger_sibling_pid;              /* process id of the oldest among younger siblings */
        pid_t older_sibling_pid;                /* process id of the youngest among older siblings */
        struct timespec start_time;             /* monotonic time */
        //struct timespec real_start_time;        /* boot based time */
        unsigned long user_time;                /* CPU time spent in user mode */
        unsigned long sys_time;                 /* CPU time spent in system mode */
        unsigned long cutime;                   /* total user time of children */
        unsigned long cstime;                   /* total system time of children */
        long uid;                               /* user id of process owner */
        char comm[16];                          /* name of program executed */
        unsigned long signal;                   /* The set of pending signals */
};



int main(int argc, char **argv)
{
	struct prinfo * info;
	if ((info = (struct prinfo *)calloc(1, sizeof(struct prinfo))) == NULL) {
		printf("Error: calloc struct prinfo\n");
		return 1;
	}
	
	sigset_t base_mask;
        sigemptyset(&base_mask);
        
	/* set signals we want to block */
        sigaddset(&base_mask, SIGUSR2);
	sigprocmask(SIG_SETMASK, &base_mask, NULL);
	
	info->pid = getpid();
	printf("pid: %d\n", info->pid);
	
	/* send SIGUSR2 to itself twice */
	kill(info->pid, SIGUSR2);
	kill(info->pid, SIGUSR2);
	
	/* suspend itself */
	kill(info->pid, SIGTSTP);
	
	return 0;
}
