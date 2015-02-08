//
//  main.c
//  share_memory
//
//  Created by pangran on 4/13/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main() {
	int shm_id, pid;
	char *shm_p, *shm_c;
	const int shm_size = 1024;
	if((shm_id = shmget(IPC_PRIVATE, shm_size, S_IRUSR | S_IWUSR)) < 0) {
        printf("Error: create shared memory failed!\n");
    }
    
	if((pid = fork()) < 0) {
		printf("Error: fork fialed!\n");
	} else if(pid == 0) {
        /* do sth in child process */

		shm_c = shmat(shm_id, NULL, 0);
		/* do sth with the shared memory here */
		sprintf(shm_c, "This is created by child process, and displayed by parent process.");
		exit(0);
	} else {
        /* do sth in parent process */
        shm_p = shmat(shm_id, NULL, 0);
        wait(NULL);
        printf("%s\n", shm_p);
    }
    return 0;
}
