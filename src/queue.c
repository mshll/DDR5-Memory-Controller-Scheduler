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
#include "queue.h"

int8_t queue_create(Queue_t **q, uint8_t max_size) {

    *q = (Queue_t *)malloc(sizeof(Queue_t));

    if (q == NULL) {
        fprintf(stderr, "%s:%d: malloc failed\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    (*q)->list = NULL;
    (*q)->size = 0;
    (*q)->max_size = max_size;

    if (doubly_ll_create(&((*q)->list)) != LL_EXIT_SUCCESS) {
        free(q);
        fprintf(stderr, "%s:%d: queue_create failed\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    return LL_EXIT_SUCCESS;
}

void queue_destroy(Queue_t **q) {

    if (*q != NULL) {
        int8_t result = doubly_ll_destroy(&((*q)->list));

        if (result != LL_EXIT_SUCCESS) {
            fprintf(stderr, "%s:%d: queue_destroy failed\n", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
        
        free(*q);
        *q = NULL;
    }
}

int8_t queue_insert_at(Queue_t **q, uint8_t index, MemoryRequest_t value) {
    if (*q == NULL || (*q)->list == NULL) {
        return LL_EXIT_USER_ERR; // Invalid queue
    }

    // queue should never overflow if main loop logic is correct
    if ( queue_is_full(*q) ) {
        fprintf(stderr, "%s:%d: Queue overflow\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    
    int8_t result = doubly_ll_insert_at(&((*q)->list), index, value);
    if (result != LL_EXIT_SUCCESS) {
        fprintf(stderr, "%s:%d: queue_insert_at failed\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    (*q)->size++;
    return result;
}

int8_t enqueue(Queue_t **q, MemoryRequest_t value) {
    if (*q == NULL || (*q)->list == NULL) {
        return LL_EXIT_USER_ERR; // Invalid queue
    }

    // queue should never overflow if main loop logic is correct
    if ( queue_is_full(*q) ) {
        fprintf(stderr, "%s:%d: Queue overflow\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    
    int8_t result = doubly_ll_insert_head(&((*q)->list), value);
    if (result != LL_EXIT_SUCCESS) {
        fprintf(stderr, "%s:%d: enqueue failed\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    (*q)->size++;
    return result;
}

MemoryRequest_t queue_delete_at(Queue_t **q, uint8_t index) {
    if (*q == NULL || (*q)->list == NULL) {
        MemoryRequest_t error_value = {.error_bit = 1};
        return error_value;
    }

    // if main loop logic is correct; we should never request from an empty queue
    if ( queue_is_empty(*q) ) {
        fprintf(stderr, "%s:%d: Dequeueing an empty queue\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    // Delete at the head of the linked list (dequeue operation)
    MemoryRequest_t stored_item = doubly_ll_delete_at(&((*q)->list), index);
    if (stored_item.error_bit == 1) {
        fprintf(stderr, "%s:%d: queue_delete_at failed\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    (*q)->size--;
    return stored_item;
}

MemoryRequest_t dequeue(Queue_t **q) {
    if (*q == NULL || (*q)->list == NULL) {
        MemoryRequest_t error_value = {.error_bit = 1};
        return error_value;
    }

    // if main loop logic is correct; we should never request from an empty queue
    if ( queue_is_empty(*q) ) {
        fprintf(stderr, "%s:%d: Dequeueing an empty queue\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    // Delete at the head of the linked list (dequeue operation)
    MemoryRequest_t stored_item = doubly_ll_delete_tail(&((*q)->list));
    if (stored_item.error_bit == 1) {
        fprintf(stderr, "%s:%d: dequeue failed\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    (*q)->size--;
    return stored_item;
}

MemoryRequest_t *queue_peek(Queue_t *q) {
    if (q == NULL || q->list == NULL) {
        return NULL;
    }

    if ( queue_is_empty(q) ) {
        return NULL;
    }

    return doubly_ll_value_at_tail(q->list);
}

bool queue_is_full(Queue_t *q) {
    if (q == NULL || q->list == NULL) {
        return false;
    }

    return (q->size == q->max_size) ? true : false;
}

bool queue_is_empty(Queue_t *q) {
    if (q == NULL || q->list == NULL) {
        return true; 
    }

    return (q->size == 0) ? true : false;
}