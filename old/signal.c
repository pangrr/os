//
//  main.c
//  signal
//
//  Created by pangran on 4/11/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//


#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int wait_mark;  /* = 1 to keep waiting, = 0 to stop waiting */

void waiting() {
    while (wait_mark != 0);
}

void stopWait() {
    wait_mark = 0;
}

int main(int argc, const char * argv[])
{

    pid_t pid1, pid2;
    
    pid1 = fork();
    if (pid1 < 0) {
        printf("Error: fork failed!\n");
        exit(1);
    } else if (pid1 > 0) {
        pid2 = fork();
        if (pid2 < 0) {
            printf("Error: fork failed!\n");
            exit(1);
        } else if (pid2 > 0) {
            /* here is sth to do for parent process after forking two child processes */
            printf("press Control + C to kill all processes\n");
            
            wait_mark = 1;
            signal(SIGINT, stopWait);
            waiting();
            
            kill(pid1, SIGINT);
            kill(pid2, SIGINT);
            wait(0);
            wait(0);
            printf("parent process is killed.\n");
            exit(0);
            
        } else {
            /* here is sth to do for child process 2 */
            wait_mark = 1;
            signal(SIGINT, stopWait);
            waiting();
            lockf(stdout, 1, 0);
            printf("child process 2 is killed.\n");
            lockf(stdout, 0, 0);
            exit(0);
        }
    } else {
        /* here is sth to do for child process 1 */
        wait_mark = 1;
        signal(SIGINT, stopWait);
        waiting();
        lockf(stdout, 1, 0);
        printf("child process 1 is killed.\n");
        lockf(stdout, 0, 0);
        exit(0);
    }
    
    return 0;
}

