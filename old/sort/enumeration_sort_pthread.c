//
//  main.c
//  enumeration_sort_parrallel
//
//  Created by pangran on 8/22/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 5

int in[N];
int out[N];



// Function called by thread to perform enumeration sort for the given element in in[].

void * locate(void * e) {
    int * i = (int *)e;
    
    int k = 0;
    
    for (int j = 0; j < N; j++) {
        if (in[j] < * i) {
            k++;
        }
    }
    
    while (out[k] != INT32_MAX) {
        k++;
    }
    
    out[k] = * i;
    
    return NULL;
}




int main(int argc, const char * argv[]) {
    
    // Input in[]
    
    char in_c[5];
    for (int i = 0; i < N; i++) {
        scanf("%s", in_c);
        in[i] = atoi(in_c);
    }
    
    // Initialize each element of out[] to IN32_MAX;
    for (int i = 0; i < N; i++) {
        out[i] = INT32_MAX;
    }
    

    
    // Create threads.
    
    pthread_t tid[N];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    for (int i = 0; i < N; i++) {
        pthread_create(&tid[i], &attr, locate, (void*)(in + i));
    }
    
    
    // Join threads
    for (int i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }
    
    
    // Print result sorted array.
    
    for (int i = 0; i < N; i++) {
        printf("%d ", out[i]);
    }
    
    
    
    
    return 0;
}
