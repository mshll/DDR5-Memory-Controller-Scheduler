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


    if (*q != NULL) {
        return LL_EXIT_USER_ERR;
    }

    *q = (Queue_t *)malloc(sizeof(Queue_t));

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

int8_t queue_destroy(Queue_t **q) {

    if (*q != NULL) {
        int8_t result = doubly_ll_destroy(&((*q)->list));

        if (result == LL_EXIT_SUCCESS) {
            free(*q);
            *q = NULL;

            return result;
        }
        
        return result;
    }

    return LL_EXIT_USER_ERR;
}

int8_t queue_insert_at(Queue_t **q, uint8_t index, MemoryRequest_t value) {
    if (*q == NULL || (*q)->list == NULL) {
        return LL_EXIT_USER_ERR; // Invalid queue
    }

    // Check if the queue is already full
    if ( queue_is_full(*q) ) {
        return LL_EXIT_USER_ERR; // Queue is full
    }

    
    int8_t result = doubly_ll_insert_at(&((*q)->list), index, value);
    if (result == LL_EXIT_SUCCESS) {
        (*q)->size++;
    }

    return result;
}

int8_t enqueue(Queue_t **q, MemoryRequest_t value) {
    if (*q == NULL || (*q)->list == NULL) {
        return LL_EXIT_USER_ERR; // Invalid queue
    }

    // Check if the queue is already full
    if ( queue_is_full(*q) ) {
        return LL_EXIT_USER_ERR; 
    }

    
    int8_t result = doubly_ll_insert_head(&((*q)->list), value);
    if (result == LL_EXIT_SUCCESS) {
        (*q)->size++;
    }

    return result;
}

MemoryRequest_t queue_delete_at(Queue_t **q, uint8_t index) {
    if (*q == NULL || (*q)->list == NULL) {
        MemoryRequest_t error_value = {0};
        return error_value;
    }

    // Check if the queue is empty
    if ( queue_is_empty(*q) ) {
        MemoryRequest_t error_value = {0};
        return error_value;
    }

    // Delete at the head of the linked list (dequeue operation)
    MemoryRequest_t stored_item = doubly_ll_delete_at(&((*q)->list), index);
    if (
        stored_item.time        == 0 &&
        stored_item.core        == 0 &&
        stored_item.operation   == 0 &&
        stored_item.byte_select == 0 &&
        stored_item.column_low  == 0 &&
        stored_item.channel     == 0 // might just need to check if channel != 0
    ) {
        return stored_item;
    }

    (*q)->size--;
    return stored_item;
}

MemoryRequest_t dequeue(Queue_t **q) {
    if (*q == NULL || (*q)->list == NULL) {
        MemoryRequest_t error_value = {0};
        return error_value;
    }

    // Check if the queue is empty
    if ( queue_is_empty(*q) ) {
        MemoryRequest_t error_value = {0};
        return error_value;
    }

    // Delete at the head of the linked list (dequeue operation)
    MemoryRequest_t stored_item = doubly_ll_delete_tail(&((*q)->list));
    if (
        stored_item.time        == 0 &&
        stored_item.core        == 0 &&
        stored_item.operation   == 0 &&
        stored_item.byte_select == 0 &&
        stored_item.column_low  == 0 &&
        stored_item.channel     == 0 // might just need to check if channel != 0
    ) {
        return stored_item;
    }

    (*q)->size--;
    return stored_item;
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