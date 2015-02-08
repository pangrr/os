#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

/* Wake up all processes waiting on the event with given eventID. */

int main (int argc, char **argv)
{
	if (argc != 2) { /* input arguments count wrong */
		printf("input error\n");
		return 0;
	}
	int eventID,sig;	
	eventID = atoi (argv[1]);
	
    /* doeventsig */
	sig = syscall(184, eventID);
	return 0;
}
