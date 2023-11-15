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

#ifndef _DOUBLY_LINKED_LIST_H_
#define _DOUBLY_LINKED_LIST_H_

/*** includes ***/
#include <stdint.h>

/*** enums ***/
enum err_code {
    LL_EXIT_SUCCESS =  0,
    LL_EXIT_FATAL   = -1,
    LL_EXIT_USER    =  1
};

/*** structs ***/
typedef struct node node_t;

typedef struct node {
    int item;
    node_t *next_node;
    node_t *prev_node;
} node_t;

typedef struct doubly_linked_list {
    node_t  *list_head; // first node
    node_t  *list_tail; // last node
    uint64_t size;      // total nodes
} doubly_linked_list_t;


/*** function declaration ***/
// functions to initialize linked list object
int8_t doubly_ll_create (
    doubly_linked_list_t **list
);
int8_t doubly_ll_destroy (
    doubly_linked_list_t **list
);

// functions to add nodes in linked list
int8_t doubly_ll_insert_at (
    doubly_linked_list_t **list, 
    uint64_t index, 
    int value
);
int8_t doubly_ll_insert_head (
    doubly_linked_list_t **list, 
    int value
);
int8_t doubly_ll_insert_tail (
    doubly_linked_list_t **list, 
    int value
);

// functions to delete nodes in linked list
int8_t doubly_ll_delete_at (
    doubly_linked_list_t **list, 
    uint64_t index
);
int8_t doubly_ll_delete_head (
    doubly_linked_list_t **list
);
int8_t doubly_ll_delete_tail (
    doubly_linked_list_t **list
);


// functions to replace valuse stored in nodes
int8_t doubly_ll_replace_at (
    doubly_linked_list_t **list,
    uint64_t index,
    int new_item
);
int8_t doubly_ll_replace_head (
    doubly_linked_list_t **list,
    int new_item
);
int8_t doubly_ll_replace_tail (
    doubly_linked_list_t **list,
    int new_item
);


// functions to retrieve values stored in nodes
int8_t doubly_ll_value_at (
    doubly_linked_list_t *list, 
    uint64_t index, 
    int *ret_val
); 
int8_t doubly_ll_value_at_head (
    doubly_linked_list_t *list, 
    int *ret_val
);
int8_t doubly_ll_value_at_tail (
    doubly_linked_list_t *list, 
    int *ret_val
);


// functions to debug linked list 
int8_t doubly_ll_print_list (
    doubly_linked_list_t *list
);
int8_t doubly_ll_list_status (
    doubly_linked_list_t *list
);
int8_t doubly_ll_node_status (
    doubly_linked_list_t *list,
    uint64_t index
);
void doubly_ll_print_err_code (
    int8_t err_code
);
uint64_t doubly_ll_size (
    doubly_linked_list_t *list
);
int8_t doubly_ll_search_for (
    doubly_linked_list_t *list,
    int value,
    uint64_t *ret_index
);


#endif 