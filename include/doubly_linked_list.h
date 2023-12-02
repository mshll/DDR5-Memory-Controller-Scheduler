/**
 * @file    doubly_linked_list.h
 * @author  your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date    2023-11-14
 * 
 * @copyright Copyright (c) 2023
 * 
**/

#ifndef __DOUBLY_LINKED_LIST_H__
#define __DOUBLY_LINKED_LIST_H__

/*** includes ***/
#include "common.h"
#include "memory_request.h"

/*** enums ***/
enum err_code {
    LL_EXIT_SUCCESS =  0,
    LL_EXIT_FATAL   = -1,
    LL_EXIT_USER_ERR =  1
};

/*** structs ***/
typedef struct node node_t;

typedef struct __attribute__((__packed__)) node {
    MemoryRequest_t item;
    node_t *next_node;
    node_t *prev_node;
} node_t;

typedef struct __attribute__((__packed__)) DoublyLinkedList {
    node_t  *list_head; // first node
    node_t  *list_tail; // last node
    uint64_t size;      // total nodes
} DoublyLinkedList_t;


/*** function declaration ***/
// functions to initialize linked list object
int8_t doubly_ll_create (
    DoublyLinkedList_t **list
);
int8_t doubly_ll_destroy (
    DoublyLinkedList_t **list
);

// functions to add nodes in linked list
int8_t doubly_ll_insert_at (
    DoublyLinkedList_t **list, 
    uint64_t index, 
    MemoryRequest_t new_item
);
int8_t doubly_ll_insert_head (
    DoublyLinkedList_t **list, 
    MemoryRequest_t new_item
);
int8_t doubly_ll_insert_tail (
    DoublyLinkedList_t **list, 
    MemoryRequest_t new_item
);

// functions to delete nodes in linked list
MemoryRequest_t doubly_ll_delete_at (
    DoublyLinkedList_t **list, 
    uint64_t index
);
MemoryRequest_t doubly_ll_delete_head (
    DoublyLinkedList_t **list
);
MemoryRequest_t doubly_ll_delete_tail (
    DoublyLinkedList_t **list
);


// functions to replace valuse stored in nodes
int8_t doubly_ll_replace_at (
    DoublyLinkedList_t **list,
    uint64_t index,
    MemoryRequest_t new_item
);
int8_t doubly_ll_replace_head (
    DoublyLinkedList_t **list,
    MemoryRequest_t new_item
);
int8_t doubly_ll_replace_tail (
    DoublyLinkedList_t **list,
    MemoryRequest_t new_item
);


// functions to retrieve values stored in nodes
MemoryRequest_t *doubly_ll_value_at (
    DoublyLinkedList_t *list, 
    uint64_t index
); 
MemoryRequest_t *doubly_ll_value_at_head (
    DoublyLinkedList_t *list
);
MemoryRequest_t *doubly_ll_value_at_tail (
    DoublyLinkedList_t *list
);


// functions to debug linked list 
int8_t doubly_ll_print_list (
    DoublyLinkedList_t *list
);
int8_t doubly_ll_list_status (
    DoublyLinkedList_t *list
);
int8_t doubly_ll_node_status (
    DoublyLinkedList_t *list,
    uint64_t index
);
void doubly_ll_print_err_code (
    int8_t err_code
);
uint64_t doubly_ll_size (
    DoublyLinkedList_t *list
);
MemoryRequest_t *doubly_ll_search_for (
    DoublyLinkedList_t *list,
    MemoryRequest_t value,
    uint64_t *ret_index
);


#endif 