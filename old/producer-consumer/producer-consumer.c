//
//  main.c
//  producer-consumer
//
//  Created by pangran on 5/14/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//
#include <time.h>   // time();
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h> // rand()
#include <unistd.h> // sleep()
#include <semaphore.h>
#include "buffer.h"

#define RANDOM_MAX 100

#define ARGUMENT_COUNT_ERROR "Error: 3 input arguments required -- sleep time, producer number, consumer number.\n"
#define SLEEP_TIME_INPUT_ERROR "Error: sleep time must be greater than 0!\n"
#define PRODUCER_COUNT_INPUT_ERROR "Error: producer number must be greater than 0!\n"
#define CONSUMER_COUNT_INPUT_ERROR "Error: consumer number must be greater than 0!\n"

const buffer_item EMPTY_ITEM  = -1;
const int BUFFER_SIZE = 5;

buffer_item buffer[BUFFER_SIZE];

int insert_position = 0;
int remove_position = 0;

int sleep_seconds;
int producer_number;
int consumer_number;

pthread_mutex_t mutex;
sem_t *full, *empty;



void *produce (void *arg);
void *consume (void *arg);
void *reset_sem (void *arg);


/*****************************************************************************/
/***                            main function                              ***/
/*****************************************************************************/


int main(int argc, const char *argv[])
{

    /* 1. Get command line arguments.
          argv[1] == sleep time in seconds,
          argv[2] == producer number, 
          argv[3] == consumer number */
    
    
    if (argc != 4) {
        printf(ARGUMENT_COUNT_ERROR);
        exit(EXIT_FAILURE);
    } else {
        sleep_seconds = atoi(argv[1]);
        producer_number = atoi(argv[2]);
        consumer_number = atoi(argv[3]);
        
        if (sleep_seconds <= 0 || producer_number <=0 || consumer_number <=0) {
            if (sleep_seconds <=0) {
                printf(SLEEP_TIME_INPUT_ERROR);
                exit(EXIT_FAILURE);
            }
            if (producer_number <=0) {
                printf(PRODUCER_COUNT_INPUT_ERROR);
                exit(EXIT_FAILURE);
            }
            if (consumer_number <=0) {
                printf(CONSUMER_COUNT_INPUT_ERROR);
                exit(EXIT_FAILURE);
            }
        }
        
    }
    
    
    /* 2. Initialize buffer, mutex, semaphore */
    

    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = EMPTY_ITEM;
    }
    
    pthread_mutex_init(&mutex, NULL);
    
    if ((empty = sem_open("empty", O_CREAT, 0644, BUFFER_SIZE)) == SEM_FAILED ) {   // semaphore initialization in Mac OS X
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if ((full = sem_open("full", O_CREAT, 0644, 0)) == SEM_FAILED ) {   // semaphore initialization in Mac OS X
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    
    
    
    
    
    /* 3. Create producer thread(s) */
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);   // get the default attributes

    
    pthread_t producer_tid[producer_number];
    
    for (int i = 0; i < producer_number; i++) {
        if (pthread_create(&producer_tid[i], &attr, produce, (void *)&i) != 0) {
            printf("ERROR failed to create producer %d", i);
            exit(EXIT_FAILURE);
        }
    }
    
    
    
    /* 4. Create consumer thread(s) */
    
    pthread_t consumer_tid[consumer_number];
    
    for (int i = 0; i < consumer_number; i++) {
        if (pthread_create(&consumer_tid[i], &attr, consume, (void *)&i) != 0) {
            printf("ERROR failed to create consumer %d", i);
            exit(EXIT_FAILURE);
        }
    }
    
    
    
    
    /* 5. Sleep */
    
    sleep(sleep_seconds);
    
    
    
    /* 6. Exit */
    
    /* kill threads */
    
    for (int i = 0; i < producer_number; i++) {
        
        if (pthread_cancel(producer_tid[i]) != 0) {
            perror("Error pthread_cancel");
            exit(EXIT_FAILURE);
        }
        
        if (pthread_join(producer_tid[i], NULL) != 0) {
            perror("Error pthread_join");
            exit(EXIT_FAILURE);
        }
    }
    
    for (int i = 0; i < consumer_number; i++) {
        
        if (pthread_cancel(consumer_tid[i]) != 0) {
            perror("Error pthread_cancel");
            exit(EXIT_FAILURE);
        }
        
        if (pthread_join(consumer_tid[i], NULL) != 0) {
            perror("Error pthread_join");
            exit(EXIT_FAILURE);
        }
    }
    
    /* close semaphores */
    
    if (sem_close(full) != 0) {
        perror("sem_close(full)");
        exit(EXIT_FAILURE);
    }
    
    if (sem_unlink("full") != 0) {
        perror("Error sem_unlink(full)");
        exit(EXIT_FAILURE);
    }

    if (sem_close(empty) != 0) {
        perror("Error sem_close(empty)");
        exit(EXIT_FAILURE);
    }
    
    if (sem_unlink("empty") != 0) {
        perror("Error sem_unlink(empty)");
        exit(EXIT_FAILURE);
    }
    
    /* destroy mutex */
    if (pthread_mutex_destroy(&mutex) != 0) {
        perror("Error pthread_mutex_destroy(&mutex)");
        exit(EXIT_FAILURE);
    }
    
    exit(0);
    return 0;
}



/*****************************************************************************/
/***                            produce function                           ***/
/*****************************************************************************/


void *produce (void *arg) {
    
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);    // make the thread cancellable
    
    int i = * (int *) arg;
    
    buffer_item item_insert;
    
    srand((unsigned int)time(NULL));  // generate a radom seed
    
    while (1) {
        srand((unsigned int)time(NULL));  // generate a radom seed

        /* sleep for a random period of time */
        sleep(rand() % 3 + 1);
        
        item_insert = rand() % RANDOM_MAX;   // generate a random int 0 ~ RANDOM_MAX -1
        
        
        
        if (sem_wait(empty) != 0) {
            perror("sem_wait(empty)");
            exit(EXIT_FAILURE);
        }
        
        if (pthread_mutex_lock(&mutex) != 0) {
            perror("pthread_mutex_lock");
            exit(EXIT_FAILURE);
        }
        
        
        
        /*** critical area begin ***/

        if (insert_item(buffer, &insert_position, item_insert) != 0) {
            printf("Error: producer %d failed to insert item into buffer!\n", i);
        } else {
            printf("producer %d: %d has been inserted\n", i, item_insert);
        }
        
        /*** critical area end ***/

        
        
        
        if (pthread_mutex_unlock(&mutex) != 0) {
            perror("pthread_mutex_unlock");
            exit(EXIT_FAILURE);
        }
        
        if (sem_post(full) != 0) {
            perror("sem_post(full)");
            exit(EXIT_FAILURE);
        }
    }
}



/*****************************************************************************/
/***                            consume function                           ***/
/*****************************************************************************/


void *consume (void *arg) {
    
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);    // make the thread cancellable
    
    
    int i = * (int *) arg;
    
    buffer_item item_remove;
    
    
    while (1) {
        srand((unsigned int)time(NULL));  // generate a radom seed

        /* sleep for a random period of time */
        sleep(rand() % 3 + 1);
        
        
        if (sem_wait(full) != 0) {
            perror("sem_wait(full)");
            exit(EXIT_FAILURE);
        }
        
        if (pthread_mutex_lock(&mutex) != 0) {
            perror("pthread_mutex_lock");
            exit(EXIT_FAILURE);
        }
        
        
        
        /*** critical area begin ***/
        
        if (remove_item(buffer, &remove_position, &item_remove) != 0) {
            printf("Error: consumer %d failed to remove item from buffer!\n", i);
        } else {
            printf("consumer %d: %d has been removed\n", i, item_remove);
        }
        
        /*** critical area end ***/
        

        if (pthread_mutex_unlock(&mutex) != 0) {
            perror("pthread_mutex_unlock");
            exit(EXIT_FAILURE);
        }
        
        if (sem_post(empty) != 0) {
            perror("sem_post(empty)");
            exit(EXIT_FAILURE);
        }
    }
}




