/* linux/include/linux/prinfo.h */


#include <linux/types.h>
#include <linux/time.h>

struct prinfo {
	long state;				/* current state of process */
	pid_t pid;				/* process id (input) */
	pid_t parent_pid;			/* process id of parent which receives SIGCHLD */
	//pid_t real_parent_pid;		/* process id of real parent */
	pid_t youngest_child_pid;		/* process id of youngest child */
	pid_t younger_sibling_pid;		/* process id of the oldest among younger siblings */
	pid_t older_sibling_pid;		/* process id of the youngest among older siblings */
	struct timespec start_time;		/* monotonic time */
	//struct timespec real_start_time;	/* boot based time */
	unsigned long user_time;		/* CPU time spent in user mode */
	unsigned long sys_time;			/* CPU time spent in system mode */
	unsigned long cutime;			/* total user time of children */
	unsigned long cstime;			/* total system time of children */
	long uid;				/* user id of process owner */
	char comm[16];				/* name of program executed */
	unsigned long signal;			/* The set of pending signals */
};

asmlinkage long sys_prinfo(struct prinfo * u_info);
