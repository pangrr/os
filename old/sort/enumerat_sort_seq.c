//
//  main.c
//  enumeration_sort_single_process
//
//  Created by pangran on 8/22/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#define N 5


int main(int argc, const char * argv[])
{

    
    // Define a blank int array to hold the input int array.
    int in[N];
    
    int out[N];
    for (int i = 0; i < N; i++) {
        out[i] = INT32_MAX;
    }
    
    // Define a blank int array to hold the sorted int array.
    
    // Define a char container to hold each input char for transforming to int later.
    char char_in[5];
    
    // Read N times of input char, transform char to int and put int to int array each time.
    for (int i = 0; i < N; i++) {
        scanf("%s", char_in);
        in[i] = atoi(char_in);
    }
    
    // Start algorithm.
    for (int i = 0; i < N; i++) {
        int k = 0;
        for (int j = 0; j < N; j++) {
            if (j != i) {
                if (in[j] < in[i]) {
                    k++;
                }
            }
        }
        
        while (out[k] != INT32_MAX) {
            k++;
        }
        
        out[k] = in[i];
    }
    
    
    
    
    
    // Print sorted array.
    
    for (int i = 0; i < N; i++) {
        printf("%d ", out[i]);
    }

    return 0;
}


