/**
 * @file    queue.h
 * @author  your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date    2023-11-15
 * 
 * @copyright Copyright (c) 2023
 * 
**/

#ifndef __QUEUE_H__
#define __QUEUE_H__

/*** includes ***/
#include "doubly_linked_list.h"
#include <stdbool.h>
#include <stdint.h>

/*** struct(s) ***/
typedef struct {
    DoublyLinkedList_t *list;
    uint64_t size;
    uint64_t max_size; // maximum size of the queue
} queue_t;

/*** function declaration(s) ***/
int8_t queue_create   (queue_t **q, uint8_t max_size);
int8_t queue_destroy  (queue_t **q);
int8_t queue_insert_at(queue_t **q, uint8_t index, MemoryRequest_t value);
int8_t enqueue(queue_t **q, MemoryRequest_t value);
MemoryRequest_t queue_delete_at(queue_t **q, uint8_t index);
MemoryRequest_t dequeue(queue_t **q);
bool   queue_is_full  (queue_t *q);
bool   queue_is_empty (queue_t *q);



#endif