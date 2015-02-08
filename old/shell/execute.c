/* This file contains 2 functions.
 * execute() and runpipe().
 * These functions deal with execute commands in child processes.
 */
#include "shell.h"

/* This function checks whether the first input argument is a commands which
 * 1. cannot be executed by execvp() properly;
 * 2. should be run in a child process.
 *
 * Such commands include 'jobs'.
 *
 * INPUT:
 *      Parsed char * argv[] that contains arguments of a command.  
 * OUTPUT:
 *      0 if no special command is found.
 *      1 if a special command is found and executed.
 *
 * Behavior Description:
 *      'jobs': print the currently running background processes in format 'process id: command'.
 * 
 * Note that all these commands can be executed piping with other commands.
 * But in this case they might not bring out the expected result.
 *
 * This function should be called firstly by any newly created child process which is meant to run a command.
 * This function is called in execute() and runpipe().
 * 
 */
    int
check_special_cmd(char * argv[])
{
    int special_cmd_found = 0;

     /* jobs */
    if(strcmp(argv[0], JOBS) == 0) 
    {
        list_pcs();
        special_cmd_found = 1;
    }

    
    return special_cmd_found;
}



/* This function is called by execute() when an input contains multiple sub-command.
 * It forks a child processes for each sub-command,
 * and build pipes between these processes for input and output transmission.
 * All of these processes should run in the foreground.
 */
    void
runpipe()
{
    /* build pipes */

    int pipe_n = CMD_N -1;
    int * pipefds;
    if ((pipefds = (int*)malloc(pipe_n * 2 * sizeof(int))) == NULL)
    {
        perror("Error malloc() in runpipe()");
        exit( EXIT_FAILURE );
    }


    for (int i = 0; i < pipe_n; i++)
    {
        if (pipe(pipefds + i*2) == -1)
        {
            perror("Error pipe() in runpipe()");
            exit(EXIT_FAILURE);
        }
    }




    cmd_t * cmd = CMD_HEAD -> next;
    
    pid_t pid1;
    int status;



    for (int i = 0; i < CMD_N; i++) /* for each cmd */
    {
        pid1 = fork();
        if (pid1 < 0)
        {
            perror( "Error fork() in runpipe()" );
            exit( EXIT_FAILURE );
        }

        /* child */
        else if (pid1 == 0)          
        {
            if (i > 0)  /* if there is a previous cmd */
            {
                if (dup2( pipefds[(i-1)*2], 0 ) == -1 )
                {
                    perror( "Error dup2() in runpipe()" );
                    exit( EXIT_FAILURE );
                }

                if (close( pipefds[(i-1)*2] ) == -1 )   /* old_fdp[0] */
                {
                    perror( "Error close() in runpipe()" );
                    exit( EXIT_FAILURE );
                }
                if (close( pipefds[(i-1)*2+1] ) == -1 ) /* old_fdp[1] */
                {
                    perror( "Error close() in runpipe()" );
                    exit( EXIT_FAILURE );
                }
                
            }
            if (i < CMD_N -1)   /* if there is a next cmd */
            {
                if (close( pipefds[i*2] ) == -1 )   /* new_fdp[0] */
                {
                    perror( "Error close() in runpipe()" );
                    exit( EXIT_FAILURE );
                }
                if (dup2( pipefds[i*2+1], 1 ) == -1 )   /* new_fdp[1] */
                {
                    perror( "Error dup2() in runpipe()" );
                    exit( EXIT_FAILURE );
                }
                if (close( pipefds[i*2+1] ) == -1 )
                {
                    perror( "Error close() in runpipe()" );
                    exit( EXIT_FAILURE );
                }
            }
            /* Execute command. */

            /* If 'exit' is called, just terminate this process. */
             if (strcmp((cmd -> argv)[0], EXIT) == 0)
                 exit( EXIT_SUCCESS );

            /* cd */
            if(strcmp((cmd -> argv)[0], CD) == 0)
            {
                chdir((cmd -> argv)[1]);
            }
            else 
            {   /* Check special command. */ 
                if (check_special_cmd(cmd -> argv) == 0)
                {
                    /* If no special command, call execvp(). */
                    if (execvp((cmd -> argv)[0], cmd -> argv ) == -1)
                    {
                        perror( cmd -> cmd );
                        exit( EXIT_FAILURE );
                    }
                }
            }
            exit( EXIT_SUCCESS );
        }

        /* parent */
        else    
        {
            if (i > 0)  /* if there is a previous cmd */
            {
                if (close( pipefds[(i-1)*2] ) == -1 )   /* old_fdp[0] */
                {
                    perror( "Error close() in runpipe()" );
                    exit( EXIT_FAILURE );
                }
                if (close( pipefds[(i-1)*2+1] ) == -1 ) /* old_fdp[1] */
                {
                    perror( "Error close() in runpipe()" );
                    exit( EXIT_FAILURE );
                }
            }
            cmd = cmd -> next;
        }
    }   // end of loop

    /* Parent process should wait for all its child processes to terminate. */
    for (int k = 0; k < CMD_N; k++)
    {
        waitpid(-1, &status, 0);
    }

    return;
}

/* The command checking and executing priority is:
 * 1. 'exit'                --\
 * 2. 'cd'                  --/ These first two sorts should run in parent process if no pipe.
 * 3. special commands  --\
 * 4. execvp()          --/ Thses last two sorts should run in a child process.
 */

    void
execute()
{
    /* Declare and initialize some variables that will be used later. */
    pid_t pid;
    int status;
    cmd_t * cmd = CMD_HEAD -> next;
    char ** argv = cmd -> argv;

     /* If only one command is inputed, check BACKGROUND status. */
    if (CMD_N == 1) 
    {
         /* 'exit' is called, just exit shell. */
        if (strcmp(argv[0], EXIT) == 0)
            exit( EXIT_SUCCESS );

         /* cd */
        if (strcmp(argv[0], CD) == 0)
        {
            chdir(argv[1]);
        }
        else 
        {   
            /* If 'exit' nor 'cd' is called, fork a child. */
            pid = fork();

            /* If fork error, print the error message.
             * Do not exit shell. 
             */
            if (pid < 0)
                perror( "Error fork() in execute()" );

            /* child */
            else if (pid == 0)
            {
                /* Execute the command.
                 * If it is not a special command, call execvp().
                 */
                if (check_special_cmd(argv) == 0)
                {
                    if (execvp(argv[0], argv) == -1)
                    {
                        perror(cmd -> cmd);
                        exit(EXIT_FAILURE);
                    }
                }
                exit(EXIT_SUCCESS);
            }

            /* parent */
            else
            {   /* If child process run in the foreground,
                 * wait it to terminate.
                 */
                 if ( BACKGROUND == 0 )
                 {
                    waitpid( pid, &status, 0 );
                    /* Regain the controlling process of foreground process group
                     * to receive terminal input and terminal generated signals.
                     */
                    if ( tcsetpgrp( STDIN_FILENO, getpid() ) == -1 )
                    {
                        perror("Error tcsetpgrp() in parent process" );
                        exit( EXIT_FAILURE );
                    }

                 }
                /* If child process run in the background,
                 * do not wait for it to termiante,
                 * instead insert the child process's information into the background process information chain. */
                 else
                {
                    if (insert_pcs(pid) == NULL)
                    {
                        perror("Error insert_pcs() in execute()");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }

    /* If there are more than one sub-command,
     * fork a child process and call runpipe() in it.
     * All sub-command should run in different child processes created by that child process.
     * They should transmit their inputs and outputs via pipes.
     */
    else       
    {   
        pid = fork();

        /* If fork error, print the error message.
         * Do not exit shell. 
         */
        if (pid < 0)
            perror("Error fork() in exucute()");
            
        /* child */
        else if (pid == 0)   
        {
            /* Because the child process runs in the foreground,
             * it should handle SIGINT and SIGTSTP as default.
             */
            signal(SIGTSTP, SIG_DFL);
            signal(SIGINT, SIG_DFL);

            runpipe();
            exit( EXIT_SUCCESS );
        }
        /* Parent should wait for the child process to termiante. */ 
        else {   
            waitpid( pid, &status, 0 );
        }
    }
    return;
}    

