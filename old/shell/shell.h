/*
 * =====================================================================================
 *
 *       Filename:  shell.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/06/2014 00:30:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef SHELL_H
#define SHELL_H

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define MAX_CMD_LEN 80
#define MAX_ARGC MAX_CMD_LEN / 2 + 1
#define EXIT "exit"
#define JOBS "jobs"
#define CD "cd"
#define PIPE "|"
#define MAX_DIR_LEN 100
#define PROMPT_NAME "John"
#define PROMPT_SIGN "$"
#define SPACE " "
#define BACKGROUND_SIGN "&"

/* define structure */
struct pcs_s
{
    struct pcs_s * next;    // a pointer to the next process_info in the chain
    pid_t pid;
    char cmd[MAX_CMD_LEN]; // exactly is user's input
};
typedef struct pcs_s pcs_t;


struct cmd_s
{
    struct cmd_s * next;
    char ** argv;
    char cmd[MAX_CMD_LEN];
};
typedef struct cmd_s cmd_t;


/* declare extern varibles */
extern pid_t SHELL_PGID;    // process group id of the shell
extern pcs_t * PCS_HEAD; // head of pcs_t chain
extern int BACKGROUND;  // whether '&' is input
extern int CMD_N;  // number of single cmd in an input
extern cmd_t * CMD_HEAD; // point to the first cmd_t of in queue
extern cmd_t * CMD_TAIL; // point to the last in cmd_t

/* declare functions defined in pcs_manager.c */
    pcs_t *
insert_pcs( pid_t pid );
    
    void
remove_pcs();

    void
list_pcs();

/* declare functions defined in input_parser.c */
    void
clear_str(char * str, int str_len);

    void
clear_cmd_queue();

    void
parse_input(char * input_buff);


/* declare functions defined in execute.c */

    void
execute();

    void
runpipe();

    int
check_special_cmd(char * argv[]);




#endif
