#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Create an empty queue
struct Queue *create_queue()
{
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
int size(struct Queue *Q)
{
    return Q->size;
}

// might need them later
// // Returns Front Element of the Queue
// struct Process frontElement(struct Queue *Q) {
//     return Q->array[Q->front];
// }

// // Returns the Rear Element of the Queue
// struct Process rearElement(struct Queue *Q) {
//     return Q->array[Q->rear];
// }

// Checks if Queue is empty or not
int is_empty(struct Queue *Q)
{
    // if the condition is true then 1 is returned else 0 is returned
    return (Q->size == 0);
}

// Checks if Queue is full or not
int is_full(struct Queue *Q)
{
    // if the condition is true then 1 is returned else 0 is returned
    return (Q->size == Q->capacity);
}

// Adding elements in Queue
void enqueue(struct Queue *Q, struct Process data)
{
    if (is_full(Q))
        printf("Queue overflow\n");
    else
    {
        Q->rear = (Q->rear + 1) % Q->capacity;
        Q->array[Q->rear] = data;
        if (Q->front == -1)
            Q->front = Q->rear;
        Q->size += 1;
    }
}

// Removes an element from front of the queue
struct Process dequeue(struct Queue *Q)
{
    struct Process data = {0};
    if (is_empty(Q))
    {
        printf("Queue is empty\n");
        return data;
    }
    data = Q->array[Q->front];
    if (Q->front == Q->rear)
    {
        Q->front = Q->rear = -1;
        Q->size = 0;
    }
    else
    {
        Q->front = (Q->front + 1) % Q->capacity;
        Q->size -= 1;
    }
    return data;
}

void delete_queue(struct Queue *Q)
{
    if (Q)
    {
        if (Q->array)
            free(Q->array);
        free(Q);
    }
}

void print_queue(struct Queue *Q)
{
    if (is_empty(Q))
    {
        printf("Queue is empty\n");
        return;
    }

    int i = Q->front;
    do
    {
        struct Process p = Q->array[i];
        printf("Time: %lu, Core: %d, Operation: %d, Address: %llx\n",
               p.time, p.core, p.operation, p.address);
        i = (i + 1) % Q->capacity;
    } while (i != (Q->rear + 1) % Q->capacity);
}