/*
 * test.c
 * This programs requires a pid_t as input and prints out the corresponding process's state
 */ 

#include <linux/unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>

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
	/* check if input argument count equals 2
	 * if input argument count does not equal 2, print out error message and return -1 */
	if (argc != 2) {
		printf("Error: input arguments count should be 2\n");
		return -1;
	}

	/* allocate memory for struct prinfo and create a pointer to it
 	 * if fail to allocate memory, print an error message and return -1 */
	struct prinfo * info;
	if ((info = (struct prinfo *)calloc(1, sizeof(struct prinfo))) == NULL) {
		printf("Error: calloc struct prinfo\n");
		return -1;
	}

	/* assign input argument to struct prinfo->pid */
	info->pid = atoi(argv[1]);

	/* call system call prinfo(struct prinfo * info) */
	int ret = syscall(181, info);

	/* check the return value of the system call
 	 * if return value is not 0, print an error message and return -1 */
	if (ret != 0) {
		printf("Error: %s\n", strerror(ret));
		return -1;
	}
	/*
	if (ret == EINVAL) {
		printf("error: pid not exists or input prinfo pointer == NULL\n");
		return -1;	
	} else if (ret == EFAULT) {
		printf("error: pointer points to invalid memory\n");
		return -1;
	} else if (ret != 0) {
		printf("error: unknown\n");
		return -1;
	}	
	*/

	/* print the value of each field in struct prinfo */ 
	printf("state: %ld\n", info->state);
	printf("pid: %d\n", info->pid);
	printf("parent pid: %d\n", info->parent_pid);
	//printf("real parent pid: %d\n", info->real_parent_pid);
	printf("youngest child pid: %d\n", info->youngest_child_pid);
	printf("younger sibling pid: %d\n", info->younger_sibling_pid);
	printf("older sibling pid: %d\n", info->older_sibling_pid);
	printf("start time: %ld\n", info->start_time.tv_sec);
	//printf("real start time: %ld\n", info->real_start_time.tv_sec);
	printf("user time: %lu\n", info->user_time);
	printf("system time: %lu\n", info->sys_time);
	printf("children user time: %lu\n", info->cutime);
	printf("children system time: %lu\n", info->cstime);
	printf("uid: %ld\n", info->uid);
	printf("comm: %s\n", info->comm);
	printf("signal: %lu\n", info->signal);

	return 0;
}
