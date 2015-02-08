#include "shell.h"

/* Find the active job with the indicated pgid.  */
job *
find_job (pid_t pgid)
{
  job *j;

  for (j = first_job; j; j = j->next)
    if (j->pgid == pgid)
      return j;
  return NULL;
}


/* Return true if all processes in the job have stopped or completed.  */
int
job_is_stopped (job *j)
{
  process *p;

  for (p = j->first_process; p; p = p->next)
    if (!p->completed && !p->stopped)
      return 0;
  return 1;
}



/* Return true if all processes in the job have completed.  */
int
job_is_completed (job *j)
{
  process *p;

  for (p = j->first_process; p; p = p->next)
    if (!p->completed)
      return 0;
  return 1;
}


/* Put job j in the foreground.  If cont is nonzero,
   restore the saved terminal modes and send the process group a
   SIGCONT signal to wake it up before we block.  */

void
put_job_in_foreground (job *j, int cont)
{
    /* Put the job into the foreground.  */
    tcsetpgrp (shell_terminal, j->pgid);

    /* Send the job a continue signal, if necessary.  */
    if (cont)
    {
      tcsetattr (shell_terminal, TCSADRAIN, &j->tmodes);
      if (kill (- j->pgid, SIGCONT) < 0)
        perror ("kill (SIGCONT)");
    }
    /* Wait for it to report.  */
    wait_for_job (j);

    /* Put the shell back in the foreground.  */
    tcsetpgrp (shell_terminal, shell_pgid);

    /* Restore the shell’s terminal modes.  */
    tcgetattr (shell_terminal, &j->tmodes);
    tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);
}




/* Put a job in the background.  If the cont argument is true, send
   the process group a SIGCONT signal to wake it up.  */

void
put_job_in_background (job *j, int cont)
{
  /* Send the job a continue signal, if necessary.  */
  if (cont)
    if (kill (-j->pgid, SIGCONT) < 0)
      perror ("kill (SIGCONT)");
}



/* Check for processes that have status information available,
   blocking until all processes in the given job have reported.  */

void
wait_for_job (job *j)
{
    while (1)
    {
        process *p = j->first_process;
      
        while (p)
        {
            if (p->status == 1)
                break;
            p = p->next;
        }
        if (!p)
            return;
    }
}




/* Format information about job status for the user to look at.  */

void
format_job_info (job *j, const char *status)
{
  fprintf (stderr, "%s %ld: %s\n", status, (long)j->pgid, j->cmd);
}





/* Create a job waiting for launch. */
job *
init_job (char * input_buffer)
{
    job *j;
    if ((j = (job *) malloc (sizeof(job))) == NULL)
    {
        perror ("malloc");
        exit (1);
    }

    parse_input (input_buffer, j);

    return j;
}






/* Mark a stopped job J as being running again.  */

void
mark_job_as_running (job *j)
{
    process *p;

    for (p = j->first_process; p; p = p->next)
        p->stopped = 0;

    j->notified = 0;
}





/* Continue the job J.  */

void
continue_job (job *j, int foreground)
{
  mark_job_as_running (j);
  if (foreground)
    put_job_in_foreground (j, 1);
  else
    put_job_in_background (j, 1);
}



// remove job from job list if all processes in that job have terminated
void
free_job(job *j)
{
    if (first_job == j)
    {
        free (j);
        return;
    }
    
    job *p = first_job;
    while (p->next)
    {
        if (p->next == j)
        {
            p->next = j->next;
            free (j);
        }
        p = p->next;
    }

    if (p == j)
    {
        free (p);
    }
    else
    {
        printf ("job not found!");
        fflush (stdout);
        exit (1);
    }
}


/* check job list and remove jobs in which all processes have terminated */
void 
update_job_list ()
{
    job *j = first_job;

    while (j)
    {
        process *p = j->first_process;

        while (p)
        {
            if (p->status == 1)
                break;
            p = p->next;
        }
        if (!p)
        {
            job *j_to_free = j;
            j = j->next;
            free_job (j_to_free);
        }
        else
            j = j->next;
            
    }
}





void
show_jobs ()
{
    update_job_list ();

    for (job *j = first_job; j; j = j->next)
    {
        for (process *p = j->first_process; p; p = p->next)
        {
            printf ("%d %d: %s\n", p->status, p->pid, p->cmd);
            fflush (stdout);
        }
    }
}

