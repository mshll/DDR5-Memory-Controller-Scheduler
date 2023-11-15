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
    doubly_linked_list_t *list;
    uint64_t size;
    uint64_t max_size; // maximum size of the queue
} queue_t;

/*** function declaration(s) ***/
int8_t queue_create   (queue_t **q, uint8_t max_size);
int8_t queue_destroy  (queue_t **q);
int8_t queue_insert_at(queue_t **q, uint8_t index, int value);
int8_t queue_delete_at(queue_t **q, uint8_t index);
bool queue_is_full  (queue_t *q);
bool queue_is_empty (queue_t *q);


//function declarations

struct Process {
    uint64_t time;
    uint32_t core;
    uint32_t operation;
    uint64_t address;
};

struct Queue {
    int front, rear;
    int capacity;
    int size;
    struct Process *array;  // store Process
};
// might need them later
struct Process front_element(struct Queue *Q);
struct Process rear_element(struct Queue *Q);
struct Queue *create_queue();
int size(struct Queue *Q);
int is_empty(struct Queue *Q);
int is_full(struct Queue *Q);
void enqueue(struct Queue *Q, struct Process data);
struct Process dequeue(struct Queue *Q);
void delete_queue(struct Queue *Q);
void print_queue(struct Queue *Q);

#endif