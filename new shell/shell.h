
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
#define PIPE_SIGN "|"
#define MAX_DIR_LEN 100
#define PROMPT_NAME "John"
#define PROMPT_SIGN "$"
#define SPACE " "
#define BACKGROUND_SIGN "&"

/* define data structures here. */


/* A process is a single process.  */
typedef struct process
{
  struct process *next;       // next process in pipeline
  char **argv;                // for exec
  char cmd[MAX_CMD_LEN];                  // command line, used for message
  pid_t pid;                  // process ID
  char completed;             // true if process has completed
  char stopped;               // true if process has stopped
  int status;                 // reported status value -1 = not launched, 0 = stopped, 1 = running
} process;

/* A job is a pipeline of processes.  */
typedef struct job
{
  struct job *next;           // next active job
  char cmd[MAX_CMD_LEN];              // command line, used for messages 
  process *first_process;     // list of processes in this job
  pid_t pgid;                 // process group ID 
  char notified;              // true if user told about stopped job 
  struct termios tmodes;      // saved terminal modes
  int infile, outfile, errfile;  // standard i/o channels
} job;

/* The active jobs are linked into a list.  This is its head.   */



/* declare extern varibles */
extern job *first_job;   // active jobs are linked, this is the head
extern pid_t shell_pgid;    // process group id of the shell
extern char foreground;  // whether '&' is input
extern pid_t shell_pgid;
extern struct termios shell_tmodes;
extern int shell_terminal;
extern int shell_is_interactive;
extern char input_buffer[MAX_CMD_LEN];   // an array of char to keep the input string.


/* Declare all functions here. */
void
clear_str (char * str, int str_len);

char **
parse_single_cmd (char *cmd);

void
parse_input (char * input_buffer, job *j);

job *
find_job (pid_t pgid);

int
job_is_stopped (job *j);

int
job_is_completed (job *j);

void
put_job_in_foreground (job *j, int cont);

void
put_job_in_background (job *j, int cont);

void
format_job_info (job *j, const char *status);

void
do_job_notification (void);

job *
init_job (char * input_buff);

void
mark_job_as_running (job *j);

void
continue_job (job *j, int foreground);

void
launch_process (process *p, pid_t pgid,
                int infile, int outfile, int errfile,
                int foreground);

void
launch_job (job *j, int foreground);

void
prompt(void);

void
init_shell(void);

void
refresh_shell(void);

void
free_job (job *j);

process *
init_process (char *cmd);


char
check_special_cmd (char **argv);

void
show_jobs ();


void
update_process_status ();

void
change_process_status_to_terminate (pid_t pid);

void
wait_for_job (job *j);

void
update_job_list ();

#endif
