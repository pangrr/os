//
//  file_reader.c
//  reader-writer
//
//  Created by pangran on 5/23/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "file_reader.h"



/* Read one line,
 then parse it to a bundle.
 This function returns 0 if all successful, otherwise it returns -1. */


int _read (const char * file_path, struct bundle cmd[MAX_CMD], int * cmd_count) {
    
    int error_flag = 0;
    
    /* open a file for read only */
    
    FILE *file;
    if ((file = fopen(file_path, "r")) == NULL) {
        perror("Erorr opening file");
        return -1;
    }
    
    
    /* start reading */
    
    char buffer[LINE_MAX_LENGTH];
    char c;
    int buf_i = 0, cmd_i = 0;
    
    while ((c = fgetc(file)) != EOF) {
        
    
        /* read one line into buffer */
        
        if (c != '\n') {
            buffer[buf_i++] = c;
        } else {    // reaches end of line, start parsing
            
            buffer[buf_i] = ' ';   // add a divider for parsing
            
            buffer[++buf_i] = '\0'; // end mark
            
            buf_i = 0;
            
            if (_parse(buffer, & cmd[cmd_i]) != 0) {
                printf("Error parsing a line\n");
                error_flag = 1;
                break;
            }
            cmd_i++;    // parsing successful, prepare for next
        }
    }
    
    * cmd_count = cmd_i;
    
    
    /* close file */
    
    if (fclose(file) != 0) {
        perror("Error closing file");
        return -1;
    }
    
    if (error_flag == 1) {
        return -1;
    }

    return 0;
}



/* Convert buffer into bundle.
   Divider is space(s) or tab(s).
   Take only the first 4 words into account.
   Neglect the left words if any.
   Return 0 if successful,
   otherwise return -1 */

int _parse (char buffer[LINE_MAX_LENGTH], struct bundle * cmd) {

    int word_head = 0, buf_i = 0, word_count = 0;
    
    char c;
    
    int divider_flag = 1;
    
    while ((c = buffer[buf_i]) != '\0') {
        
        if (c == ' ' || c == '\t') {
            
            buffer[buf_i] = '\0';
            
            if (divider_flag == 0) {
                word_count++;
    
                if (word_count == 1) {
                    (* cmd).number = atoi(& buffer[word_head]); // add error handler here
                } else if (word_count == 2) {
                    if (buffer[word_head] == 'R' || buffer[word_head] == 'r') {
                        (* cmd).type = 0;
                    } else if (buffer[word_head] == 'W' || buffer[word_head] == 'w') {
                        (* cmd).type = 1;
                    } else {
                        printf("Error second word should be r/R/w/W\n");
                        return -1;  // stop parsing on error
                    }
                } else if (word_count == 3) {
                    (* cmd).start_time = atoi(& buffer[word_head]); // add error handler here
                } else if (word_count == 4) {
                    (* cmd).duration = atoi(& buffer[word_head]); // add error handler here
                } else {
                    break;  // stop parsing if reaches the 5th word
                }
                
            }
            
            
            divider_flag = 1;

        } else {    // non-divider character
            
            if (divider_flag == 1) {
                word_head = buf_i;
            }
            
            divider_flag = 0;
        }
        
        buf_i++;
    }
    
    return 0;
    
}


