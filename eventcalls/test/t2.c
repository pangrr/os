#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* Wait on an event with given eventID. */

int main(int argc, char **argv)
{
	if(argc != 2) {
		printf("Input error\n");
		return 0;
	}

	pid_t pid;
	int eventID;
	int wait;

	pid = getpid();
	printf("The pid is: %d\n", pid);
	eventID = atoi (argv[1]);
	
    /* doeventwait */
	wait = syscall(183, eventID);
	if (wait == -1) {
		printf("Fail in waiting\n");
		return 0;
	}
	printf("end\n");
	return 0;
}
