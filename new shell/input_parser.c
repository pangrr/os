
/* This file provide functions to parse input command line,
 * and finally make a job variable ready to launch. */


#include "shell.h"

    void
clear_str( char str[], int str_len )
{
    for( int i = 0; i < str_len; i++ )
        str[i] = '\0';
    return;
}




/* Return a char **argv from a command line that has only one command in. */

char **
parse_single_cmd (char * single_cmd)
{   
    char ** argv;
    if(( argv = (char **) malloc( MAX_ARGC * sizeof(char *) ) ) == NULL )
    {
        perror ("malloc");
        exit (1);
    }
    
    char * saveptr;
    char * single_argv;

    /* iterate through single_cmd to build argv[] */
    int i = 0;
    for( single_argv = strtok_r (single_cmd, SPACE, &saveptr ); single_argv; single_argv = strtok_r( NULL, SPACE, &saveptr ) )
    {
        /* set BACKGROUND = 1 encountering BACKGROUND_SIGN */
        if (strcmp (single_argv, BACKGROUND_SIGN ) == 0 )
            foreground = 0;
        else
            foreground = 1;
        
        if ((argv[i] = (char *) malloc ( sizeof(char) )) == NULL )
        {
            perror ("malloc");
            exit (1);
        }
        strcpy (argv[i++], single_argv);
    }

    /* end argv with NULL */
    if (foreground == 0)   // cut '&'
        argv[i-1] = NULL;
    else
        argv[i] = NULL;
    
    return argv;
}





/* Build processes for a job based on given input. */

void    
parse_input (char * input_buffer, job *j)
{
    int process_count = 0;
    char cmd_line[MAX_CMD_LEN];
    clear_str(j-> cmd, MAX_CMD_LEN);
    clear_str (cmd_line, MAX_CMD_LEN);    // remmeber to clear string before use, or something might be left
   
    /* Remove '\n' in the end. */
    input_buffer[strlen (input_buffer) - 1] = 0;

    strcpy (cmd_line, input_buffer);
    strcpy (j->cmd, cmd_line);

    char * saveptr;
    char * single_cmd;

    process * p;

    for (single_cmd = strtok_r (cmd_line, PIPE_SIGN, & saveptr); single_cmd; single_cmd = strtok_r (NULL, PIPE_SIGN, & saveptr))
    {
        /* Create a new process and add it to the process list of the job. */
        process * new_process = init_process (single_cmd);

        if (!process_count)
        {
            j->first_process = new_process;
            p = new_process;
        }
        else
        {
            p->next = new_process;
            p = new_process;
        }     
        process_count++;

    }

}




