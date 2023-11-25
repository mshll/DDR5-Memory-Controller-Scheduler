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
typedef struct Queue {
    DoublyLinkedList_t *list;
    uint64_t size;
    uint64_t max_size; // maximum size of the queue
} Queue_t;

/*** function declaration(s) ***/
int8_t queue_create(Queue_t **q, uint8_t max_size);
void queue_destroy(Queue_t **q);
int8_t queue_insert_at(Queue_t **q, uint8_t index, MemoryRequest_t value);
int8_t enqueue(Queue_t **q, MemoryRequest_t value);
MemoryRequest_t queue_delete_at(Queue_t **q, uint8_t index);
MemoryRequest_t dequeue(Queue_t **q);
MemoryRequest_t *queue_peek(Queue_t *q);
MemoryRequest_t *queue_peek_at(Queue_t *q, uint8_t index);
bool queue_is_full(Queue_t *q);
bool queue_is_empty(Queue_t *q);

#endif