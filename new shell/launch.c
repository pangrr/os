#include "shell.h"




void
launch_process (process *p, pid_t pgid,
                int infile, int outfile, int errfile,
                int foreground)
{
   
    pid_t pid;

    if (shell_is_interactive)
    {
        /* Put the process into the process group and give the process group
            the terminal, if appropriate.
            This has to be done both by the shell and in the individual
            child processes because of potential race conditions.  */
        pid = getpid ();



        if (!pgid) pgid = pid;
        setpgid (pid, pgid);
        if (foreground)
            tcsetpgrp (shell_terminal, pgid);

        /* Set the handling for job control signals back to the default.  */
        signal (SIGINT, SIG_DFL);
        signal (SIGQUIT, SIG_DFL);
        signal (SIGTSTP, SIG_DFL);
        signal (SIGTTIN, SIG_DFL);
        signal (SIGTTOU, SIG_DFL);
        signal (SIGCHLD, SIG_DFL);
    }

    /* Set the standard input/output channels of the new process.  */
    if (infile != STDIN_FILENO)
    {
        dup2 (infile, STDIN_FILENO);
        close (infile);
    }
    if (outfile != STDOUT_FILENO)
    {
        dup2 (outfile, STDOUT_FILENO);
        close (outfile);
    }
    if (errfile != STDERR_FILENO)
    {
        dup2 (errfile, STDERR_FILENO);
        close (errfile);
    }

    /* Exec the new process.  Make sure we exit.  */
    if (!check_special_cmd (p->argv))
    {
        if (execvp (p->argv[0], p->argv) == -1)
        {
            perror ("execvp");
            exit (1);
        }
    }
    exit (0);
}




void
launch_job (job *j, int foreground)
{
    /* Check for special cmd if there is only one process. */

    if (!j->first_process->next)
    {
        if (check_special_cmd (j->first_process->argv))
            return;
    }

    process *p;
    pid_t pid;
    int mypipe[2], infile, outfile;

    infile = j->infile;
    for (p = j->first_process; p; p = p->next)
    {
        /* Set up pipes, if necessary.  */
        if (p->next)
        {
            if (pipe (mypipe) < 0)
            {
                perror ("pipe");
                exit (1);
            }
            outfile = mypipe[1];
        }
        else
            outfile = j->outfile;

        /* Fork the child processes.  */
        pid = fork ();
        if (pid < 0)
        {
            /* The fork failed.  */
            perror ("fork");
            exit (1);
        }

        else if (pid == 0)
        {
            /* This is the child process.  */
            launch_process (p, j->pgid, infile, outfile, j->errfile, foreground);
        }
        
        else
        {
            /* This is the parent process.  */
            p->pid = pid;
            if (shell_is_interactive)
            {
                if (!j->pgid)
                    j->pgid = pid;
                
                setpgid (pid, j->pgid);
                
                if (foreground)
                    tcsetpgrp (shell_terminal, j->pgid);
            }
            
        }

        /* Clean up after pipes.  */
        if (infile != j->infile)
            close (infile);
        if (outfile != j->outfile)
            close (outfile);
        infile = mypipe[0];
    }

    if (!foreground)
        format_job_info (j, "launched");

    if (!shell_is_interactive)
    {
        wait_for_job (j);
    }
    else if (foreground)
    {
        put_job_in_foreground (j, 0);
    }
    else
    {
        put_job_in_background (j, 0);
    }
}


