/* This .c file contains the main() function and prompt() function.
 */

#include "shell.h"

/* Declare golobal variables here.
 */
int BACKGROUND; // programm runs at front in default
int CMD_N; // number of sigle cmd in an inpiut
pcs_t * PCS_HEAD;   // a pointer to the header (which is an empty node) of process information chain
cmd_t * CMD_HEAD;   // a pointer to the header of command information chain
cmd_t * CMD_TAIL;   // a pointer to the last node of the command information chain


/* This function simply print the shell prompt
 * and the working directory on the terminal.
 * It is called by function main().
 */
void
prompt (void)
{
    char working_dir[MAX_DIR_LEN];
    char working_folder[MAX_DIR_LEN];
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




    int
main ( void )
{

    /* Set a signal handler to remove a background process information from the chain 
     * when this background process terminates. */
    signal( SIGCHLD, remove_pcs );
 
    
    /* Initialize pcs_t * PCS_HEAD by malloc(). */
    if ( ( PCS_HEAD = malloc( sizeof( pcs_t ) ) ) == NULL )
    {
        perror( "Error PCS_HEAD = malloc() in main()" );
        exit( EXIT_FAILURE );
    }
    PCS_HEAD -> next = NULL;

    /* Initialize cmd_t * CMD_HEAD by malloc(). */
    if ( ( CMD_HEAD = malloc( sizeof( cmd_t ) ) ) == NULL )
    {
        perror( "Error CMD_HEAD = malloc( cmd_t ) in main()" );
        exit( EXIT_FAILURE );
    }
    CMD_HEAD -> next = NULL;
    /* Initilize CMD_TAIL by point it to CMD_HEAD. */
    CMD_TAIL = CMD_HEAD;    


    /* This is the main loop that keeps print prompt
     * and get ready for the next command untill exit is called or receiving interruption. */
    while (1) 
    {
        /* Create an array of char to keep the input string. */
        char input_buff[MAX_CMD_LEN];
        /* Set all elements of the array to null.
         * function clear_str() is defined in input_parser.c */
        clear_str(input_buff, MAX_CMD_LEN);  
        
        /* Initialize some variables used later. */
        int input_len;  // record the number of char in the input
        BACKGROUND = 0; // a flag to record wheter the current command should run in background(1) or foreground(0)
        CMD_N = 1;  // record the number of sub-command an input contains
        
        /* Empty the command information queue. */
        clear_cmd_queue();
        
        /* Print prompt. */
        prompt();

        /* Wait for input '\n'.
         * Then save all input into input_buff,
         * get the number of characters in the input (exclude '\n').
         * The reason to exclude '\n' is that it is nothing more than a signal for the program to start reading input. */
        input_len = read( STDIN_FILENO, input_buff, MAX_CMD_LEN ) - 1;
        
        /* If user has typed in something other than '\n'
         * start parsing the input and try execute the input. */
        if( input_len > 0 )
        {
            /* Parse input string into execvp() friendly format.
             * Build command queue.
             * Set background flag.
             * This function is defined in input_parser.c
             */
            parse_input(input_buff);
            
            /* Fork a child process or more to execute the parsed input command.
             * This function may use pipe if input comtains more than one sub-command.
             */
            execute();
        }
    }
    return EXIT_SUCCESS;
}

