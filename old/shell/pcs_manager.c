
#include "shell.h"

    pcs_t *
insert_pcs ( pid_t pid )
{ 
    pcs_t * pcs;
    if ( ( pcs = malloc ( sizeof ( pcs_t ) ) ) == NULL )
        return NULL;
    pcs -> pid = pid;
    strcpy( pcs -> cmd, CMD_HEAD -> next -> cmd );
    
    /* add pcs to the head of pcs chain */
    pcs -> next = PCS_HEAD -> next;
    PCS_HEAD -> next = pcs;
    return pcs;
}




    void
remove_pcs ()
{ 
    pid_t pid;
    int status;
    

    while (1)  // think about it
    { 
        pid = waitpid ( -1, & status, WNOHANG );

        if (pid > 0)    /* find an terminated child */
        {
            pcs_t * pcs = PCS_HEAD;
        
            while ( pcs -> next != NULL )
            { 
                /* if the pid is found */
                if ( pcs -> next -> pid == pid )
                {
                    pcs_t * _p;
                    _p = pcs -> next;
                    pcs -> next = pcs -> next -> next;
                    free( _p ); 
                    return;
                }   
                else
                    pcs = pcs -> next;
            }
        }
        else    /* no child remain or all children running */
            return;
    }
    return;
}




    void
list_pcs ()
{    

    pcs_t * pcs = PCS_HEAD -> next;
    while ( pcs != NULL )
    { 
        printf( "%d : %s\n", pcs -> pid, pcs -> cmd );
        pcs = pcs -> next;
    }   
    return; 
}





