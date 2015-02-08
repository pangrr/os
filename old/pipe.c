//
//  main.c
//  pipe
//
//  Created by pangran on 4/11/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int main() {
	pid_t pid1, pid2;
	int fd[2];
	char str_write[50], str_read[50];
	pipe(fd);
    
	if((pid1 = fork()) < 0) {
		printf("Error: fork failed!\n");
        exit(1);
	} else if(pid1 == 0) {
        lockf(fd[1], 1, 0);
        sprintf(str_write, "child process 1 is sending a message.\n");
        write(fd[1], str_write, 50);
        sleep(1);
        lockf(fd[1], 0, 0);
        exit(0);
	} else {
		if((pid2 = fork()) < 0) {
			printf("Error: fork failed!");
            exit(1);
		} else if(pid2 == 0) {
			lockf(fd[1], 1, 0);
			sprintf(str_write, "child process 2 is sending a message.\n");
			write(fd[1], str_write, 50);
			lockf(fd[1], 0, 0);
			exit(0);
		} else {
			wait(0);
			if(read(fd[0], str_read, 50) < 0) {
				printf("Error: read from pipe failed!\n");
			} else {
				printf("%s", str_read);
			}
			wait(0);
			if(read(fd[0], str_read, 50) < 0) {
				printf("Error: read from pipe failed!\n");
			} else {
				printf("%s", str_read);
			}
			exit(0);
		}
    }
}

