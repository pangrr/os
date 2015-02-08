//
//  file_reader.h
//  reader-writer
//
//  Created by pangran on 5/23/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//

#ifndef reader_writer_file_reader_h
#define reader_writer_file_reader_h

#define LINE_MAX_LENGTH 20
#define MAX_CMD 20

struct bundle {
    int number; // 1,2,...
    int type;   // reader = 0, writer = 1
    int start_time; // when reader/writer start trying to read/write
    int duration;   // how long reader/writer takes to finish reading/writing
};

/* In this function, a text line is ended with '\n'.
   This function is to read a whole text file,
   and put non-space or non-tab characters of the same line 
   into a char[4] array, so that a char[4][MAX_LINE] matrix is built.
   This function returns 0 if all successful, otherwise it returns -1. */

int _read (const char *file_path, struct bundle * cmd, int * cmd_count);

int _parse (char * buffer, struct bundle * cmd);


#endif
