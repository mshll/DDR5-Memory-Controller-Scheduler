/**
 * @file    doubly_linked_list.c
 * @author  your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date    2023-11-14
 * 
 * @copyright Copyright (c) 2023
 * 
**/

/*** includes ***/
#include "common.h"
#include "doubly_linked_list.h"
#include <stdlib.h>
#include <stdio.h>


/*** 
 * functions to initialize linked list object 
***/
int8_t doubly_ll_create (
    doubly_linked_list_t **list
) {
    /**
     * @brief  initialized a linked list object 
     * 
     * @param  list     the linked list object
     * @param  value    the value to store in the first node
     * @return int8_t   error code; -1 = fatal error, 0 = no error, 1 = user error
    **/

    // check if list already exist
    if ( *list != NULL ){
        return LL_EXIT_USER;
    }

    // create list
    if (
        ( *list = (doubly_linked_list_t *) malloc(sizeof(doubly_linked_list_t)) )
        == NULL
    ) {
        return LL_EXIT_FATAL;
    }

    // init list
    (*list)->list_head = NULL;
    (*list)->list_tail = NULL;
    (*list)->size = 0;

    return LL_EXIT_SUCCESS;
}


int8_t doubly_ll_destroy (
    doubly_linked_list_t **list
) {
    /**
     * @brief  frees all the memory allocated to a linked list
     * 
     * @param  list     the linked list to free
     * @return int8_t   error code; -1 = fatal error, 0 = no error, 1 = user error
    **/
    
    // check if list exist
    if ( *list == NULL ){
        return LL_EXIT_USER;
    }

    // traverse list
    node_t *temp = NULL;

    while ( (*list)->list_head != NULL ) {
        temp = (*list)->list_head->next_node;
        free( (*list)->list_head );
        (*list)->list_head = temp;

        (*list)->size--;
    }

    // something went wrong
    if ( (*list)->size != 0 ) {
        return LL_EXIT_FATAL;
    }

    // danglers begone
    (*list)->list_tail = NULL;
    (*list)->list_head = NULL;

    free(*list);
    *list = NULL;
    temp = NULL;

    return LL_EXIT_SUCCESS;
}


/*** 
 * functions to add nodes in linked list 
***/
int8_t doubly_ll_insert_at (
    doubly_linked_list_t **list, 
    uint64_t index, 
    int value
) {
    /**
     * @brief  inserts node at index given by user
     * 
     * @param  list     linked list
     * @param  index    index to insert
     * @return int8_t   error code
    **/

    // check if list exist
    if ( *list == NULL ) {
        return LL_EXIT_USER;
    }

    // check if index is in range of list
    if ( index > (*list)->size ) {
        return LL_EXIT_USER;
    }

    // create node
    node_t *new_node = NULL;

    if (
        ( new_node = (node_t *) malloc(sizeof(node_t)) )
        == NULL
    ) {
        return LL_EXIT_FATAL;
    }
    new_node->item = value;

    // insert at head
    if ( index == 0 ) {
        new_node->next_node = (*list)->list_head;
        new_node->prev_node = NULL;
        (*list)->list_head = new_node;

        if ( (*list)->size == 0 ) {
            (*list)->list_tail = new_node;
        }

        (*list)->size++;

        return LL_EXIT_SUCCESS;
    }

    // insert at tail
    if ( index == (*list)->size ) {
        new_node->prev_node = (*list)->list_tail; // prev points to old tail
        new_node->next_node = NULL;

        (*list)->list_tail->next_node = new_node;
        (*list)->list_tail = new_node;

        (*list)->size++;

        return LL_EXIT_SUCCESS;
    }

    // insert inbetween list
    node_t *current_node = (*list)->list_head;

    /*** current node points to node right before insertion point
     *   when index equals 1. This is required to insert node at appropriate location.
    ***/
    while ( index != 1 ) {
        current_node = current_node->next_node;
        index--;
    }

    // new node points to next node and prev node (aka current node)
    new_node->next_node = current_node->next_node;
    new_node->prev_node = current_node;

    // have the old next node point back new node (the node that took its place)
    current_node->next_node->prev_node = new_node;
    // current node (aka prev node) point to new node
    current_node->next_node = new_node;

    (*list)->size++;

    return LL_EXIT_SUCCESS;
}


int8_t doubly_ll_push (
    doubly_linked_list_t **list, 
    int value
) {
    /**
     * @brief  adds node to head of list
     * 
     * @param  list     the linked list object
     * @param  value    the value of the new node
     * @return int8_t   error code
    **/
   
    // check if list exist
    if ( *list == NULL ) {
        return LL_EXIT_USER;
    }

    // create node
    node_t *new_node = NULL;
    if (
        ( new_node = (node_t *) malloc(sizeof(node_t)) )
        == NULL
    ) {
        return LL_EXIT_FATAL;
    }
    new_node->item = value;
    new_node->prev_node = NULL;

    
    // case 1; list is empty
    if ( (*list)->size == 0 ) {
        new_node->next_node = NULL;
        (*list)->list_head = new_node;
        (*list)->list_tail = new_node;
        (*list)->size++;

        return LL_EXIT_SUCCESS;
    }

    // insert to head
    new_node->next_node = (*list)->list_head; // point to old head
    (*list)->list_head->prev_node = new_node;
    (*list)->list_head = new_node;

    (*list)->size++;

    return LL_EXIT_SUCCESS;
}


int8_t doubly_ll_append (
    doubly_linked_list_t **list, 
    int value
) {
    /**
     * @brief  adds node to tail of list
     * 
     * @param  list     the linked list object
     * @param  value    the vlaue of the new node
     * @return int8_t   error code
    **/
    
    // check if list exist
    if ( *list == NULL ) {
        return LL_EXIT_USER;
    }

    // create node
    node_t *new_node = NULL;
    if (
        ( new_node = (node_t *) malloc(sizeof(node_t)) )
        == NULL
    ) {
        return LL_EXIT_FATAL;
    }
    new_node->item = value;
    new_node->next_node = NULL;

    // attach to tail if list is empty
    if ( (*list)->list_tail == NULL ) {
        new_node->prev_node = NULL;
        (*list)->list_tail = new_node;
        (*list)->list_head = new_node;
        (*list)->size++;

        return LL_EXIT_SUCCESS;
    }

    // attach to tail
    new_node->prev_node = (*list)->list_tail;
    (*list)->list_tail->next_node = new_node;
    (*list)->list_tail = new_node;
    (*list)->size++;

    return LL_EXIT_SUCCESS;
}


/*** 
 * functions to delete nodes in linked list 
***/
int8_t doubly_ll_delete_at (
    doubly_linked_list_t **list, 
    uint64_t index
) {
    /**
     * @brief  deletes node at a given index.
     * 
     * @param  list 
     * @param  index 
     * @return int8_t 
    **/

    // list existane or empty list
    if ( (*list) == NULL || (*list)->size == 0 ) {
        return LL_EXIT_USER;
    }

    // check if index is in range
    if ( index > (*list)->size - 1 ) {
        return LL_EXIT_USER;
    }

    // traverse list
    node_t *temp = NULL;
    node_t *current_node = (*list)->list_head;
    
    // if deleting head node
    if ( index == 0 ) {
        (*list)->list_head = current_node->next_node;

        // check if node is the last one in list
        if ( current_node == (*list)->list_tail ) {
            (*list)->list_tail = NULL;
            (*list)->size--;
            free(current_node);

            return LL_EXIT_SUCCESS;
        }

        (*list)->list_head->prev_node = NULL;
        (*list)->size--;
        free(current_node);

        return LL_EXIT_SUCCESS;
    }

    // if deleting last node
    if ( index == (*list)->size - 1 ) {
        temp = (*list)->list_tail;

        // case 1; only one node in list
        if ( (*list)->size == 1 ) {
            (*list)->list_head = NULL;
            (*list)->list_tail = NULL;
            (*list)->size--;

            free(temp);
            return LL_EXIT_SUCCESS;
        }        

        (*list)->list_tail = temp->prev_node;
        (*list)->list_tail->next_node = NULL;
        (*list)->size--;

        free(temp);
        return LL_EXIT_SUCCESS;
    }

    // current_node stops at node right before node at index
    while ( index != 1 ) {
        current_node = current_node->next_node;
        index--;
    }

    // temp points to node that will be deleted
    temp = current_node->next_node;

    // have current node point to temp's next node cuz temp node will be del
    current_node->next_node = temp->next_node;

    // have next node point back to current node cuz temp node will be del
    temp->next_node->prev_node = current_node;

    (*list)->size--;
    free(temp);

    return LL_EXIT_SUCCESS;
}


int8_t doubly_ll_pop (
    doubly_linked_list_t **list
) {
    /**
     * @brief  deletes the first node in list
     * 
     * @param  list 
     * @return int8_t 
    **/

    // check if list exist or is empty
    if ( *list == NULL || (*list)->size == 0 ) {
        return LL_EXIT_USER;
    }
    
    // point to node to delete
    node_t *temp = (*list)->list_head;
    (*list)->list_head = temp->next_node;

    // check if theres only one node
    if ( temp == (*list)->list_tail ) {
        (*list)->list_tail = NULL;

        // delete node
        free(temp);
        (*list)->size--;

        return LL_EXIT_SUCCESS;
    }

    // make sure new head node does not point to old head node
    (*list)->list_head->prev_node = NULL;

    // delete node
    free(temp);
    (*list)->size--;

    return LL_EXIT_SUCCESS;
}


int8_t doubly_ll_remove (
    doubly_linked_list_t **list
) {
    /**
     * @brief  deletes the last node in list
     * 
     * @param  list 
     * @return int8_t 
    **/
    
    // check if list exist or is empty
    if ( *list == NULL || (*list)->size == 0 ) {
        return LL_EXIT_USER;
    }

    node_t *temp = NULL;
    temp = (*list)->list_tail;

    // case 1; only one node in list
    if ( (*list)->size == 1 ) {
        (*list)->list_head = NULL;
        (*list)->list_tail = NULL;
        (*list)->size--;

        free(temp);
        return LL_EXIT_SUCCESS;
    }        

    (*list)->list_tail = temp->prev_node;
    (*list)->list_tail->next_node = NULL;
    (*list)->size--;

    free(temp);

    return LL_EXIT_SUCCESS;
}


/***
 * functions to replace values stored in nodes
***/
int8_t doubly_ll_replace_at (
    doubly_linked_list_t **list,
    uint64_t index,
    int new_item
) {
    /**
     * @brief  replaces node at index given by user
     * 
     * @param  list     linked list
     * @param  index    index to insert
     * @param  new_item the new value that will replace the old
     * @return int8_t   error code
    **/

    // check if list exist
    if ( *list == NULL ) {
        return LL_EXIT_USER;
    }

    // check if index is in range of list
    if ( index + 1 > (*list)->size ) {
        return LL_EXIT_USER;
    }

    // replace at head
    if ( index == 0 ) {

        (*list)->list_head->item = new_item;
        return LL_EXIT_SUCCESS;
    }

    // insert at tail
    if ( index == (*list)->size ) {

        (*list)->list_tail->item = new_item;
        return LL_EXIT_SUCCESS;
    }

    // insert inbetween list
    node_t *current_node = (*list)->list_head;

    /*** current node points to node whos value will be replace
    ***/
    while ( index != 0 ) {
        current_node = current_node->next_node;
        index--;
    }

    current_node->item = new_item;

    return LL_EXIT_SUCCESS;
}


int8_t doubly_ll_replace_head (
    doubly_linked_list_t **list,
    int new_item
) {
    /**
     * @brief  replaces node at index given by user
     * 
     * @param  list     linked list
     * @param  new_item the new value that will replace the old
     * @return int8_t   error code
    **/

    // check if list exist or is empty
    if ( *list == NULL || (*list)->size == 0 ) {
        return LL_EXIT_USER;
    }

    (*list)->list_head->item = new_item;
    return LL_EXIT_SUCCESS;
}


int8_t doubly_ll_replace_tail (
    doubly_linked_list_t **list,
    int new_item
) {
    /**
     * @brief  replaces node at index given by user
     * 
     * @param  list     linked list
     * @param  new_item the new value that will replace the old
     * @return int8_t   error code
    **/

    // check if list exist or is empty
    if ( *list == NULL || (*list)->size == 0 ) {
        return LL_EXIT_USER;
    }

    (*list)->list_tail->item = new_item;

    return LL_EXIT_SUCCESS;
}


/*** 
 * functions to retrieve values stored in nodes 
***/
int8_t doubly_ll_value_at (
    doubly_linked_list_t *list, 
    uint64_t index, 
    int *ret_val
) {
    /**
     * @brief  retrieve the value from a node at the given index 
     * 
     * @param  list     linked list
     * @param  index    the node location
     * @param  ret_val  the variable where the value gets stored in
     * @return int8_t   error code
    **/
    
    // check if list exist or is empty
    if ( list == NULL || list->size == 0 ) {
        return LL_EXIT_USER;
    }

    // check if index is in range of list
    if ( index + 1 > list->size ) {
        return LL_EXIT_USER;
    }

    // replace at head
    if ( index == 0 ) {

        *ret_val = list->list_head->item;
        return LL_EXIT_SUCCESS;
    }

    // insert at tail
    if ( index == list->size ) {

        *ret_val = list->list_tail->item;
        return LL_EXIT_SUCCESS;
    }

    // insert inbetween list
    node_t *current_node = list->list_head;

    /*** current node points to node whos value will be replace
    ***/
    while ( index != 0 ) {
        current_node = current_node->next_node;
        index--;
    }

    *ret_val = current_node->item;

    return LL_EXIT_SUCCESS;
}


int8_t doubly_ll_value_at_head (
    doubly_linked_list_t *list, 
    int *ret_val
) {
    /**
     * @brief  retrieve the value from the first node in the list
     * 
     * @param  list     linked list
     * @param  ret_val  the variable where the value gets stored in
     * @return int8_t   error code
    **/
    
    // check if list exist or is empty
    if ( list == NULL || list->size == 0 ) {
        return LL_EXIT_USER;
    }

    *ret_val = list->list_head->item;
    return LL_EXIT_SUCCESS;
}


int8_t doubly_ll_value_at_tail (
    doubly_linked_list_t *list, 
    int *ret_val
) {
    /**
     * @brief  retrieve the value from the last node in the list 
     * 
     * @param  list     linked list
     * @param  ret_val  the variable where the value gets stored in
     * @return int8_t   error code
    **/
    
    // check if list exist or is empty
    if ( list == NULL || list->size == 0 ) {
        return LL_EXIT_USER;
    }

    *ret_val = list->list_tail->item;
    return LL_EXIT_SUCCESS;
}


/*** 
 * functions to debug linked list 
***/
int8_t doubly_ll_print_list (
    doubly_linked_list_t *list
) {
    /**
     * @brief  prints all the values stored in the linked list.
     * 
     * @param  list     the linked list object
     * @return int8_t   error code
    **/
    // check if list is empty
    if ( list == NULL ) {
        return LL_EXIT_USER;
    }

    /*** although "list" is a local variable, "list_head" is not and is being reference.
     *   hence, modifing "list_head" in this function will result in a permanent change.
    ***/
    node_t *temp = list->list_head;

    fprintf(stdout, "\nlinked list: ");
    
    // traverse linked list
    while ( temp != NULL ) {
        fprintf(stdout, "%d ", temp->item);
        temp = temp->next_node;
    }

    fprintf(stdout, "\n");
    
    return LL_EXIT_SUCCESS;
}


int8_t doubly_ll_list_status (
    doubly_linked_list_t *list
) {
    /**
     * @brief  prints all members of list obj.
     * 
     * @param  list 
     * @return int8_t 
    **/
    
    // check if list exist
    if ( list == NULL ) {
        return LL_EXIT_USER;
    }

    printf(
        "\nlinked_list_t:\n"
        "\tlist @%p\n"
        "\thead @%p\n"
        "\ttail @%p\n"
        "\tsize = %lu\n",
        list,
        list->list_head,
        list->list_tail,
        list->size
    );

    return LL_EXIT_SUCCESS;
}


int8_t doubly_ll_node_status (
    doubly_linked_list_t *list,
    uint64_t index
) {
    /**
     * @brief  prints all members of node obj.
     * 
     * @param  list 
     * @return int8_t 
    **/
    
    // check if list exist
    if ( list == NULL ) {
        return LL_EXIT_USER;
    }

    if ( index > list->size - 1 ) {
        return LL_EXIT_USER;
    }

    node_t *current_node = list->list_head;

    while ( index != 0 ) {
        current_node = current_node->next_node;
        index--;
    }

    printf(
        "\nnode_t: @%p\n"
        "\tnext @%p\n"
        "\tprev @%p\n"
        "\titem = %d\n",
        current_node,
        current_node->next_node,
        current_node->prev_node,
        current_node->item
    );

    return LL_EXIT_SUCCESS;

}

void doubly_ll_print_err_code (
    int8_t err_code
) {
    /**
     * @brief  prints a description of the error code returned
     *         from the linked list API
     * 
     * @param  err_code 
     * @return int8_t  
    **/

    switch ( err_code ) {
        case LL_EXIT_SUCCESS:
            printf("\nno error occurred\n");
            break;
        case LL_EXIT_FATAL:
            printf("\nmalloc failed\n");
            break;
        case LL_EXIT_USER:
            printf("\nimproper use of linked list API\n");
            break;
        default:
            printf(
                "\nthe value: %d, does not get return "
                "from any linked list API function\n",
                err_code
            );
            break;
    }

}


uint64_t doubly_ll_size (
    doubly_linked_list_t *list
) {
    /**
     * @brief  returns the total number of nodes in linked list
     * 
     * @param  list         the linked list object
     * @return uint64_t     the total number of nodes
    **/

    // check if list exist
    if ( list == NULL ) {
        return LL_EXIT_SUCCESS;
    }
    
    return list->size;
}


int8_t doubly_ll_search_for (
    doubly_linked_list_t *list,
    int value,
    uint64_t *ret_index
) {
    /**
     * @brief  returns the index of the *first* node whos value matches
     *         the value passed by the user.  
     * 
     * @param  list     linked list
     * @param  value    the value to search for in list
     * @return int8_t   exit status; each value represents
     *                  what happen during the function call
    **/
    
    // check if list exist or is empty
    if ( list == NULL || list->size == 0 ) {
        return LL_EXIT_USER;
    }

    // index of node with matching item
    uint64_t current_index = 0;

    // cursor node
    node_t *current_node = list->list_head;

    // traverse list
    while ( current_node != NULL ) {

        if ( current_node->item == value ) {

            *ret_index = current_index;
            return LL_EXIT_SUCCESS;
        }

        // search next node
        current_node = current_node->next_node;
        current_index++;
    }

    // no match found
    return LL_EXIT_FATAL;
}