#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

/* Creat event, then wait on it. */

int main()
{	
    pid_t pid;
	int eventID;
	/* creat event */
	eventID = syscall(181);
	printf("the event ID is %d\n", eventID);
	
	pid = getpid();
	printf("pid: %d\n", pid);
	
    /* doeventwait */
	syscall(183, eventID);
	return 0;
}
