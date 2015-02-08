//
//  main.c
//  shell
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

#define MAX_LEN 80
#define EXIT "exit"
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  cmdHint
 *  Description:  print a command hint whenever a command line is expected.
 * =====================================================================================
 */
void cmdHint() {
    printf("COMMAND -> ");
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  exitHint
 *  Description:  print a message when control+c is pressed.
 * =====================================================================================
 */
void exitHint() {
    printf("\nEnter '%s' to quit shell.\n", EXIT);
    cmdHint();
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  parse
 *  Description:  read input command and prepare for execvp()
 * =====================================================================================
 */

void parse (char cmd[], ssize_t cmd_len, char *argv[], int *background)
{
	int arg_begin = 1;
	int arg_n = 0;
	int cmd_n;
    
	for (cmd_n = 0; cmd_n <  cmd_len; cmd_n++) {
		switch (cmd[cmd_n]) {
			case '\t':
			case ' ':
				arg_begin = 1;
				cmd[cmd_n] = '\0';
				*background = 0;
				break;
			case '&':
				if (arg_begin == 1) {
					arg_begin = 0;
					*background = 1;
					argv[arg_n] = &cmd[cmd_n];
				} else {
					*background = 0;
				}
				break;
			default:
				if (arg_begin == 1) {
					arg_begin = 0;
					argv[arg_n++] =  &cmd[cmd_n];
				}
				*background = 0;
				break;
		}
	}
	/* argv must end with a pointer to NULL for execvp() to use */
	argv[arg_n] = '\0';
	return;
}		/* -----  end of function parse  ----- */


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  execute
 *  Description:  fork a child process and invoke execvp() in it
 * =====================================================================================
 */
void execute (char *argv[], int *background)
{
	pid_t pid;
    int status;
    
	if ((pid = fork()) < 0) {
		printf("Error: fork failed!\n");
		exit(1);
	} else if (pid == 0) {
        /* do something in child process here */
        signal(SIGINT, SIG_IGN);    /* igore control+c in child process */
		if (execvp(*argv, argv) < 0) {
			printf("Error: execute command failed!\n");
			exit(1);
		} else {
			exit(0);
		}
	} else if (pid > 0) {
        /* do something in parent process here */
        signal(SIGINT, exitHint);
		if (*background == 0) {
			waitpid(pid, &status, 0);   
	/* Use waitpid() instead of wait() to ensure wait. 
	 * The reason is as follows. *
	 * If a child process is forked and terminated in background (parent process does not wait for its termination, *
	 * this unwaited child precess, called zombie process, remains in the memory with its basic information(pid, etc.).
	 * The next time when the parent process calls an unspecified wait(),
	 * it’s possible that the zombie process is waited and freed from memory
	 * before the newly forked child process.
	 * Thus a new loop begins while the child process from previous loop might yet been terminated.
	 * This child process might terminate and become a zombie.  */

		}
	}
	return;
}		/* -----  end of function execute  ----- */



/*
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:
 * =====================================================================================
 */
int main (void)
{
	char cmd[MAX_LEN];
    char *argv[MAX_LEN / 2 + 1];
    int background = 0;
    
    setbuf(stdout, NULL);
    
	while (1) {
        background = 0;

        signal(SIGINT, exitHint);   /* print hint to exit */
        
		cmdHint();
		
        scanf ("%[^\n]%*c", cmd);
        
        if (strcmp(cmd, EXIT) == 0) exit(0);
		
        parse(cmd, strlen(cmd), argv, &background);
		
        execute(argv, &background);
	}
	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
