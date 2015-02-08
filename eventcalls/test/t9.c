#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    if (argc != 5) { /* input arguments count wrong */
        printf("input error\n");
        return 0;
    }
    pid_t pid = getpid();

    /* Read the input arguements */
    int uid_p = atoi(argv[1]);
    int eventID = atoi(argv[2]);
    int UIDFlag_change = atoi(argv[3]);
    int GIDFlag_change = atoi(argv[4]);

    uid_t *UID;
    UID = malloc(sizeof(uid_t));
    gid_t *GID;
    GID = malloc(sizeof(gid_t));
    int *UIDFlag;
    UIDFlag = malloc(sizeof(int));
    int *GIDFlag;
    GIDFlag = malloc(sizeof(int));

	/* Change the effective uid of this task */
	seteuid(uid_p);

	/* Change the UIDFlag & GIDFlag of the event with given eventID */
    if(syscall(211, eventID, UIDFlag_change, GIDFlag_change) == -1) {
        printf("Fail in changing UID & GID\n");
        return 0;
    }
        
	/* Read the info of the event */
	if(syscall(214, eventID, UID, GID, UIDFlag, GIDFlag) == -1) {    
        printf("doeventstat fail\n");
        return 0;
    }
    printf("This event ID is: %d\n UID: %d; GID: %d; UIDFlag: %d; GIDFlag: %d\n",eventID, *UID, *GID, *UIDFlag, *GIDFlag);

	return 0;
}

