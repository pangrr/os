
#include "shell.h"


    void
clear_str( char str[], int str_len )
{
    for( int i = 0; i < str_len; i++ )
        str[i] = '\0';
    return;
}



    char **
parse_single_cmd( char * single_cmd )
{   
    char ** argv;
    if(( argv = (char **) malloc( MAX_ARGC * sizeof(char *) ) ) == NULL )  // why * MAX_ARGC ??
    {
        perror("Error argv = malloc() in parse_single_cmd()");
        exit(EXIT_FAILURE);
    }
    
    char * saveptr;
    char * single_argv;

    /* iterate through single_cmd to build argv[] */
    int i = 0;
    for( single_argv = strtok_r( single_cmd, SPACE, &saveptr ); single_argv; single_argv = strtok_r( NULL, SPACE, &saveptr ) )
    {
        /* set BACKGROUND = 1 encountering BACKGROUND_SIGN */
        if( strcmp( single_argv, BACKGROUND_SIGN ) == 0 )
            BACKGROUND = 1;
        else
            BACKGROUND = 0;
        
        if( (argv[i] = (char *) malloc( sizeof(char) )) == NULL )
        {
            perror("Error argv[i] = malloc() in parse_single_cmd");
            exit(EXIT_FAILURE);
        }
        strcpy( argv[i++], single_argv );
    }

    /* end argv with NULL */
    if( BACKGROUND == 1 )   // cut '&'
        argv[i-1] = NULL;
    else
        argv[i] = NULL;
    
    return argv;
}




    void
insert_cmd( char * single_cmd )
{ 
    cmd_t * cmd;

    /* build a new cmd */
    if(( cmd = malloc( sizeof( cmd_t )) ) == NULL )
    {
        perror("Error cmd = malloc() in insert_cmd()");
        exit(EXIT_FAILURE);
    }
    strcpy( cmd-> cmd, single_cmd );
    
    cmd -> argv = parse_single_cmd( single_cmd );
    
    cmd -> next = CMD_TAIL -> next;
    CMD_TAIL -> next = cmd;
    CMD_TAIL = cmd;
    
    return;
}



    void    
parse_input (char * input_buff)
{
    char input[MAX_CMD_LEN];
    clear_str( input, MAX_CMD_LEN );    // !! remmeber to clear string before use, or something might be left
    
    strcpy( input, input_buff );

    input[strlen(input) - 1] = '\0';    // remove '\n' at the tail
    char * saveptr;
    char * single_cmd;
    CMD_N = 0;

    for ( single_cmd = strtok_r( input, PIPE, & saveptr ); single_cmd; single_cmd = strtok_r( NULL, PIPE, & saveptr ) )
    {
        insert_cmd( single_cmd );
        CMD_N++;
    }
    return;
}





    void
clear_cmd_queue ()
{   
    cmd_t * cmd;
    while ( (cmd = CMD_HEAD -> next) != NULL )
    {
        CMD_HEAD -> next = cmd -> next;
        free( cmd );
    }
    CMD_TAIL = CMD_HEAD;
    return;
}       /* -----  end of function clear_cmd_stacke ----- */




