//
//  buffer.h
//  producer-consumer
//
//  Created by pangran on 5/14/14.
//  Copyright (c) 2014 pangran. All rights reserved.
//

#ifndef producer_consumer_buffer_h
#define producer_consumer_buffer_h

typedef int buffer_item;

extern const int BUFFER_SIZE;

extern const int EMPTY_ITEM;

int insert_item(buffer_item * buffer, int * insert_position, buffer_item item);

int remove_item (buffer_item * buffer, int * remove_position, buffer_item *item);

#endif
