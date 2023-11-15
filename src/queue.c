/**
 * @file    queue.c
 * @author  your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date    2023-11-15
 * 
 * @copyright Copyright (c) 2023
 * 
**/

/*** inclueds ***/
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "queue.h"

int8_t queue_create(queue_t **q, uint8_t max_size) {


    if ( *q != NULL ) {
        return LL_EXIT_USER;
    }

    *q = (queue_t *)malloc(sizeof(queue_t));

    if (q == NULL) {
        return LL_EXIT_FATAL; // Allocation failed
    }

    (*q)->list = NULL;
    (*q)->size = 0;
    (*q)->max_size = max_size;

    if (doubly_ll_create(&((*q)->list)) != LL_EXIT_SUCCESS) {
        free(q);
        return LL_EXIT_FATAL; // List creation failed
    }

    return LL_EXIT_SUCCESS;
}

int8_t queue_destroy(queue_t **q) {

    if (*q != NULL) {
        doubly_ll_destroy(&((*q)->list));
        free(*q);
        *q = NULL;

        return LL_EXIT_SUCCESS;
    }

    return LL_EXIT_USER;
}

bool queue_is_full(queue_t *q) {
    if (q == NULL || q->list == NULL) {
        return LL_EXIT_USER; // Invalid queue
    }

    return (q->size == q->max_size) ? true : false;
}

bool queue_is_empty(queue_t *q) {
    if (q == NULL || q->list == NULL) {
        return LL_EXIT_USER; // Invalid queue
    }

    return (q->size == 0) ? true : false;
}

int8_t queue_insert_at(queue_t **q, uint8_t index, int value) {
    if (*q == NULL || (*q)->list == NULL) {
        return LL_EXIT_USER; // Invalid queue
    }

    // Check if the queue is already full
    if ( queue_is_full(*q) ) {
        return LL_EXIT_FATAL; // Queue is full
    }

    
    int8_t result = doubly_ll_insert_at(&((*q)->list), index, value);
    if (result == LL_EXIT_SUCCESS) {
        (*q)->size++;
    }

    return result;
}

int8_t queue_delete_at(queue_t **q, uint8_t index) {
    if (*q == NULL || (*q)->list == NULL) {
        return LL_EXIT_USER; // Invalid queue
    }

    // Check if the queue is empty
    if ( queue_is_empty(*q) ) {
        return LL_EXIT_FATAL; // Queue is empty
    }

    // Delete at the head of the linked list (dequeue operation)
    int8_t result = doubly_ll_delete_at(&((*q)->list), index);
    if (result == LL_EXIT_SUCCESS) {
        (*q)->size--;
    }

    return result;
}

// Create an empty queue
struct Queue *create_queue() {
    struct Queue *Q = malloc(sizeof(struct Queue));
    if (!Q)
        return NULL;
    Q->capacity = 16;
    Q->front = Q->rear = -1;
    Q->size = 0;
    Q->array = malloc(Q->capacity * sizeof(struct Process));

    if (!Q->array)
        return NULL;
    return Q;
}

// Returns queue size
int size(struct Queue *Q) {
    return Q->size;
}

// might need them later
// Returns Front Element of the Queue
struct Process front_element(struct Queue *Q) {
    return Q->array[Q->front];
}

// Returns the Rear Element of the Queue
struct Process rear_element(struct Queue *Q) {
    return Q->array[Q->rear];
}

// Checks if Queue is empty or not
int is_empty(struct Queue *Q) {
    // if the condition is true then 1 is returned else 0 is returned
    return (Q->size == 0);
}

// Checks if Queue is full or not
int is_full(struct Queue *Q) {
    // if the condition is true then 1 is returned else 0 is returned
    return (Q->size == Q->capacity);
}

// Adding elements in Queue
void enqueue(struct Queue *Q, struct Process data) {
    if (is_full(Q))
        printf("Queue overflow\n");
    else {
        Q->rear = (Q->rear + 1) % Q->capacity;
        Q->array[Q->rear] = data;
        if (Q->front == -1)
            Q->front = Q->rear;
        Q->size += 1;
    }
}

// Removes an element from front of the queue
struct Process dequeue(struct Queue *Q) {
    struct Process data = {0};
    if (is_empty(Q)) {
        printf("Queue is empty\n");
        return data;
    }
    data = Q->array[Q->front];
    if (Q->front == Q->rear) {
        Q->front = Q->rear = -1;
        Q->size = 0;
    }
    else {
        Q->front = (Q->front + 1) % Q->capacity;
        Q->size -= 1;
    }
    return data;
}

void delete_queue(struct Queue *Q) {
    if (Q) {
        if (Q->array)
            free(Q->array);
        free(Q);
    }
}

void print_queue(struct Queue *Q) {
    if (is_empty(Q)) {
        printf("Queue is empty\n");
        return;
    }

    int i = Q->front;
    do {
        struct Process p = Q->array[i];
        printf("Time: %lu, Core: %d, Operation: %d, Address: %lx\n",
               p.time, p.core, p.operation, p.address);
        i = (i + 1) % Q->capacity;
    } while (i != (Q->rear + 1) % Q->capacity);
}