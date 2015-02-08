#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	if (argc != 5) { /* input arguments count wrong. */
		printf("input error\n");
		return 0;
	}
	
    /* Creat *UID, *GID, *UIDFlag, *GIDFlag to recieve the infomation of the event. */
	uid_t *UID, UID_in;
	UID = malloc(sizeof(uid_t));
	gid_t *GID, GID_in;
	GID = malloc(sizeof(gid_t));
	int *UIDFlag, UIDFlag_in;
	UIDFlag = malloc(sizeof(int));
	int *GIDFlag, GIDFlag_in;
	GIDFlag = malloc(sizeof(int));
	int eventID;
	
	/* Read the input arguments. */
	UID_in = atoi(argv[1]);
	GID_in = atoi(argv[2]);
	UIDFlag_in = atoi(argv[3]);
	GIDFlag_in = atoi(argv[4]);

    /* Create an event. */
	eventID = syscall(181);
	
	/* Change UID & GID. */
	if(syscall(205, eventID, UID_in, GID_in) == -1) {	
		printf("doeventchown fail\n");
		return 0;
	}

	/* Change UIDFlag & GIDFlag. */
	if(syscall(211, eventID, UIDFlag_in, GIDFlag_in) == -1) {	
		printf("doeventchmod fail\n");
		return 0;
	}

	/* Read the info of the created event. */
	if(syscall(214, eventID, UID, GID, UIDFlag, GIDFlag) == -1) {    
        printf("doeventstat fail\n");
        return 0;
    }
    
    printf("This event ID is: %d\n UID: %d; GID: %d; UIDFlag: %d; GIDFlag: %d\n",eventID, *UID, *GID, *UIDFlag, *GIDFlag);
	
	return 0;
}
