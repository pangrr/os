//
//  buffer.c
//  producer-consumer
//
//  Created by pangran on 5/18/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//

#include <stdio.h>
#include "buffer.h"

int insert_item(buffer_item *buffer, int *insert_position, buffer_item item) {
    /* Insert item into buffer.
     Return 0 if successful, otherwise
     return -1 indicating an error condition. */
    
    /* check if the insert_position is empty */
    if (buffer[*insert_position] == EMPTY_ITEM) {
        buffer[*insert_position] = item;
        *insert_position = (*insert_position + 1) % BUFFER_SIZE;
        return 0;
    } else {
        return -1;
    }
    
}

int remove_item (buffer_item *buffer, int *remove_position, buffer_item *item) {
    /* Remove an object from buffer.
     Place it in item.
     Return 0 if successful, otherwise
     return -1 indicating an error condition. */
    
    /* check if the remove_position is non-empty */
    if (buffer[*remove_position] != EMPTY_ITEM) {
        *item = buffer[*remove_position];
        buffer[*remove_position] = EMPTY_ITEM;
        *remove_position = (*remove_position + 1) % BUFFER_SIZE;
        return 0;
    } else {
        return -1;
    }
}

