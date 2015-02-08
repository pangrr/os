#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* Wake up all processes waiting on the event with given eventID then close this event. */

int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("Input error\n");
        return 0;
    }
    int eventID;
	eventID = atoi (argv[1]);
	syscall(182, eventID);

	return 0;
}
