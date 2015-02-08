#include "shell.h"

/* Create an empty process. */
process *
init_process (char *cmd)
{
    process * new_process;
    if ((new_process = (process *) malloc (sizeof(process))) == NULL)
    {
        perror ("malloc");
        exit (1);
    }
    clear_str (new_process->cmd, MAX_CMD_LEN );    // remmeber to clear string before use, or something might be left
    strcpy (new_process -> cmd, cmd);
    new_process->argv = parse_single_cmd(cmd);

    return new_process;
}



/* Check and execute for special commands. */

char
check_special_cmd (char **argv)
{
    if (strcmp (argv[0], EXIT) == 0)
        exit (0);
        
    if (strcmp (argv[0], CD) == 0)
    {
        chdir (argv[1]);
        return 1;
    }

    if (strcmp (argv[0], JOBS) == 0)
    {
        show_jobs ();
        return 1;
    }

    return 0;
}


/* Update process state if shell receive a SIGCHLD. */
void
update_process_status ()
{
    pid_t pid;
    int status;
    
    while (1)
    { 
        pid = waitpid ( -1, & status, WNOHANG );

        if (pid > 0)    /* find an terminated child */
        {
           change_process_status_to_terminate (pid);
        }
        else    /* no child remain or all children running */
            return;
    }
}

/* change status of given process to 0 */
void
change_process_status_to_terminate (pid_t pid)
{
    job *j = first_job;
    while (j)
    {
        for (process *p = j->first_process; p; p = p->next)
        {
            if (p->pid == pid)
            {
                p->status = 0;
                return;
            }
        }
    }
    if (!j)
    {
        printf ("process %d not found!\n", pid);
        fflush (stdout);
        exit (1);
    }
}


