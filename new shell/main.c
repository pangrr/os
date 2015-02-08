/* This .c file contains the main() function and prompt() function.
 */

#include "shell.h"

/* Declare golobal variables here.
 */
char foreground;
pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;
char input_buffer[MAX_CMD_LEN];   // an array of char to keep the input string.
job *first_job;   // active jobs are linked, this is the head





/* This function simply print the shell prompt
 * and the working directory on the terminal.
 * It is called by function main().
 */
    void
prompt (void)
{
    char working_dir[MAX_DIR_LEN];
    char working_folder[MAX_DIR_LEN];
    clear_str (working_dir, MAX_DIR_LEN);
    clear_str (working_folder, MAX_DIR_LEN);

    if (getcwd (working_dir, sizeof (working_dir)) == NULL )  // get current working directory
    {
        perror ("getcwd");
        exit (1);
    }

    for (int i = strlen(working_dir) - 1; i >= 0; i--)
    {
        if (working_dir[i] == '/')
        {
            int k = 0;
            for (int j = i + 1; j < strlen(working_dir); j++)
            {
                working_folder[k] = working_dir[j];
                k++;
            }
            break;
        }
    }

    char *user_name = getenv("USER");
    if (user_name == NULL)
    {
        printf ("? %s %s ", working_folder, PROMPT_SIGN);
        fflush (stdout);
    }
    else
    {
        printf ("%s %s %s ", user_name, working_folder, PROMPT_SIGN);
        fflush (stdout);
    }
} 




/* Make sure the shell is running interactively as the foreground job
   before proceeding. */
void
init_shell (void)
{

    /* See if we are running interactively.  */
    shell_terminal = STDIN_FILENO;
    shell_is_interactive = isatty (shell_terminal);

    if (shell_is_interactive)
    {
        /* Loop until we are in the foreground.  */
        while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
            kill (- shell_pgid, SIGTTIN);

        /* Ignore interactive and job-control signals.  */
        signal (SIGINT, SIG_IGN);
        signal (SIGQUIT, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        signal (SIGCHLD, update_process_status);

        /* Put ourselves in our own process group.  */
        shell_pgid = getpid ();
        if (setpgid (shell_pgid, shell_pgid) < 0)
        {
          perror ("Couldn't put the shell in its own process group");
          exit (1);
        }

        /* Grab control of the terminal.  */
        tcsetpgrp (shell_terminal, shell_pgid);

        /* Save default terminal attributes for shell.  */
        tcgetattr (shell_terminal, &shell_tmodes);

    }        
}





int
main (void)
{
    init_shell();

    while (1) 
    {
        init_shell();

        clear_str(input_buffer, MAX_CMD_LEN);  //Set all elements of the array to null. Function clear_str() is defined in input_parser.c
        
        /* Print prompt when shell is ready. */
        prompt();

        
        /* Read from input. */
        int input_len = read (STDIN_FILENO, input_buffer, MAX_CMD_LEN) - 1;

        
        /* If user has typed in something other than '\n'
         * start parsing the input and try execute the input. */
        if (input_len > 0 )
        {
            job *j = init_job (input_buffer);
            if (!first_job)
            {
                first_job = j;
            }
            else
            {
                j->next = first_job;
                first_job = j;
            }

            
            /* Fork a child process or more to execute the parsed input command.
             * This function may use pipe if input comtains more than one sub-command. */
            launch_job (j, foreground);
        }
    }
    return EXIT_SUCCESS;
}

