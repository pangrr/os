//
//  main.c
//  pthread
//
//  Created by pangran on 4/23/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define M 3
#define N 3
#define K 2
#define THREADS_NUM M * N

int A[M][K] = {{1, 2}, {3, 4}, {5, 6}};
int B[K][N] = {{1, 2, 3}, {4, 5, 6}};
int C[M][N];

struct coordinate{int x, y;};

/* function called by thread to calculate C[i][j]*/
void* calculateElement(void* coord) {
    int i, j, k;
    
    struct coordinate* c = (struct coordinate*)coord;
    i = c -> x;
    j = c -> y;
    C[i][j] = 0;
    
    /* calculate C[i][j]*/
    for (k = 0; k < K; k++) {
        C[i][j] += A[i][k] * B[k][j];
    }
    
    return NULL;
}

/* print result C */
void printC() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d\t", C[i][j]);
        }
        printf("\n");
    }
}



int main(int argc, const char * argv[])
{

    int i, j;
    /* prepare a tid container */
    pthread_t tid[THREADS_NUM];
    
    /* prepare a attribute container */
    pthread_attr_t attr;
    
    /* get default attributes */
    pthread_attr_init(&attr);
    
    /* create threads */
    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            /* allocate momory for new structure e */
            struct coordinate* coord = (struct coordinate*) malloc(sizeof(struct coordinate));
            
            /* assign value to coordinate */
            coord -> x = i;
            coord -> y = j;
            
            /* create a thread to calculate C[i][j] */
            pthread_create(&tid[i * N + j], &attr, calculateElement, (void*)coord);
        }
    }
    
    /* join every thread */
    for (i = 0; i < THREADS_NUM; i++) {
        pthread_join(tid[i], NULL);
    }
    
    printC();
    return 0;
}

