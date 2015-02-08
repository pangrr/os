//
//  main.c
//  reader-writer
//
//  Created by pangran on 5/23/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//

#include <stdio.h>
#include "file_reader.h"
#include <stdlib.h> // EXIT_FAILURE
#include <pthread.h> // mutex
#include <unistd.h> // sleep

const char * file_path;
struct bundle cmd[MAX_CMD];

pthread_mutex_t read_wait_mutex, write_mutex, write_wait_mutex, read_write_mutex;
int read_wait_count, write_wait_count;


void * thread_function (void * arg);



/*****************************************************************************/
/***                            main function                              ***/
/*****************************************************************************/



int main(int argc, const char * argv[])
{
    
    /*===== 1. Get command line arguments.
     argv[1] == file path =====*/
    
    
    if (argc != 2) {
        printf("Error: a file path should be input as an argument.\n");
        exit(EXIT_FAILURE);
    } else {
        file_path = argv[1];
    }


    /*===== 3. read commands from file into cmd[][] =====*/
    int cmd_count;
    
    if (_read(file_path, cmd, & cmd_count) != 0) {
        exit(EXIT_FAILURE);
    }
    
    
    
    /*===== 4. initialize mutex =====*/
    
    if (pthread_mutex_init(&read_wait_mutex, NULL) != 0) {
        perror("Error pthread_mutex_init(&read_wait_mutex)");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_mutex_init(&write_wait_mutex, NULL) != 0) {
        perror("Error pthread_mutex_init(&write_wait_mutex)");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_mutex_init(&read_write_mutex, NULL) != 0) {
        perror("Error pthread_mutex_init(&read_write_mutex)");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_mutex_init(&write_mutex, NULL) != 0) {
        perror("Error pthread_mutex_init(&write_mutex)");
        exit(EXIT_FAILURE);
    }
    
    read_wait_count = 0;
    write_wait_count = 0;

    
    /*===== 3. execute commands =====*/
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_t pthread_tid[cmd_count];
        
    for (int i = 0; i < cmd_count; i++) {
        
        /* create a thread */
        
        if (pthread_create(&pthread_tid[i], &attr, thread_function, (void *) & cmd[i])) {
            perror("Error create pthread");
            exit(EXIT_FAILURE);
        }
    }
    
    
    /* 4. exit */
    
    for (int i = 0; i < cmd_count; i++) {
        if (pthread_join(pthread_tid[i], NULL) != 0) {
            perror("Error pthread_join");
            exit(EXIT_FAILURE);
        }
    }
    
    if (pthread_mutex_destroy(&read_wait_mutex) != 0) {
        perror("Error pthread_mutex_destroy(&read_wait_mutex)");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_mutex_destroy(&write_wait_mutex) != 0) {
        perror("Error pthread_mutex_destroy(&write_wait_mutex)");
        exit(EXIT_FAILURE);
    }

    
    if (pthread_mutex_destroy(&read_write_mutex) != 0) {
        perror("Error pthread_mutex_destroy(&read_write_mutex)");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_mutex_destroy(&write_mutex) != 0) {
        perror("Error pthread_mutex_destroy(&write_mutex)");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}

/*****************************************************************************/
/***                    function thread_function                           ***/
/*****************************************************************************/

void * thread_function (void * arg) {
    
    struct bundle bundle_receive = * (struct bundle *) arg;

    
    printf("%d threat created.\n", bundle_receive.number);
    
    
    sleep(bundle_receive.start_time);
    
    switch (bundle_receive.type) {
            
            /* if its a writer */
        case 1:
            
            /* wait for write_wait_mutex */
            
            printf("%d waiting for write permission...\n", bundle_receive.number);
            
            if (pthread_mutex_lock(&write_wait_mutex) != 0) {
                perror("Error pthread_mutex_lock(&write_wait_mutex)");
                exit(EXIT_FAILURE);
            }
            
        
            /* increase write_wait_count */
            
            write_wait_count++;
            
            /* first writer should wait for read_write mutex */
            
            if (write_wait_count == 1) {
                if (pthread_mutex_lock(&read_write_mutex) != 0) {
                    perror("Error pthread_mutex_lock(&read_write_mutex)");
                    exit(EXIT_FAILURE);
                }
            }
            
            /* release write_wait_mutex for other writer to enter the waiting list and modify write_wait_count */
            if (pthread_mutex_unlock(&write_wait_mutex) != 0) {
                perror("Error pthread_mutex_unlock(&write_wait_mutex)");
                exit(EXIT_FAILURE);
            }
            
            
            
            
            
            /* wait for write_mutex to start writing
             during the process other writer can enter the waiting list */
            if (pthread_mutex_lock(&write_mutex) != 0) {
                perror("Error pthread_mutex_lock(&write_mutex)");
                exit(EXIT_FAILURE);
            }
            
            /* start writing */
            
            printf("%d writing start...\n", bundle_receive.number);
            
            sleep(bundle_receive.duration);
            
            printf("%d writing done.\n", bundle_receive.number);
            
            /* end writing */
            
            /* release write_mutex for other writer in the waiting list to start writing */
            if (pthread_mutex_unlock(&write_mutex) != 0) {
                perror("Error pthread_mutex_unlock(&write_mutex)");
                exit(EXIT_FAILURE);
            }
            
            
            
            
            /* wait for write_wait_mutex to decrease write_wait_count */
            if (pthread_mutex_lock(&write_wait_mutex) != 0) {
                perror("Error pthread_mutex_lock(&write_wait_mutex)");
                exit(EXIT_FAILURE);
            }
            
            /* decrease write_wait_count */
            write_wait_count--;
            
            /* the last writer should release read_write_mutex */
            if (write_wait_count == 0) {
                if (pthread_mutex_unlock(&read_write_mutex) != 0) {
                    perror("Error pthread_mutex_unlock(&read_write_mutex)");
                    exit(EXIT_FAILURE);
                }
            }
            
            /* release write_wait_mutex for other writer to modify write_wait_count */
            if (pthread_mutex_unlock(&write_wait_mutex) != 0) {
                perror("Error pthread_mutex_unlock(&write_wait_mutex)");
                exit(EXIT_FAILURE);
            }
            
            break;
            

            
            
            
            /* if its a reader */
        case 0:
            
            /* wait for read_wait_mutex to enter the read waiting list and increase read_wait_count */
            
            printf("%d waiting for read permission...\n", bundle_receive.number);
            
            if (pthread_mutex_lock(&read_wait_mutex) != 0) {
                perror("Error pthread_mutex_lock(&read_wait_mutex)");
                exit(EXIT_FAILURE);
            }
            
            /* increase read_count */
            read_wait_count++;
            
            /* first reader should wait for read_write_mutex */
            
            if (read_wait_count ==1) {
                if (pthread_mutex_lock(&read_write_mutex) != 0) {
                    perror("Error pthread_mutex_lock(&read_write_mutex)");
                    exit(EXIT_FAILURE);
                }
            }
            
            /* release read_wait_mutex for other reader to enter the read waiting list and modify read_wait_count */
            if (pthread_mutex_unlock(&read_wait_mutex) != 0) {
                perror("Error pthread_mutex_unlock(&read_wait_mutex)");
                exit(EXIT_FAILURE);
            }
            
            
            
            
            
            /* start reading 
             meanwhile other reader can enter the read waiting list
             and start reading */

            printf("%d reading start...\n", bundle_receive.number);
            
            sleep(bundle_receive.duration);
            
            printf("%d reading done.\n", bundle_receive.number);
            
            /* end reading */
            
            
            
            
            

            /* wait for read_wait_mutex to decrease read_wait_count */
            if (pthread_mutex_lock(&read_wait_mutex) != 0) {
                perror("Error pthread_mutex_lock(&read_wait_mutex)");
                exit(EXIT_FAILURE);
            }
            
            /* decrease read_count */
            
            read_wait_count--;
            
            /* last reader should release read_write_mutex */
            if (read_wait_count == 0) {
                if (pthread_mutex_unlock(&read_write_mutex) != 0) {
                    perror("Error pthread_mutex_unlock(&read_write_mutex)");
                    exit(EXIT_FAILURE);
                }
            }
            
            /* release read_wait_mutex for other reader to enter read waiting list and modify read_wait_count */
            
            if (pthread_mutex_unlock(&read_wait_mutex) != 0) {
                perror("Error pthread_mutex_unlock(&read_wait_mutex)");
                exit(EXIT_FAILURE);
            }
            
            break;
    }
    
    
    return NULL;
}

















