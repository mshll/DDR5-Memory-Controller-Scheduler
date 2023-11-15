#ifndef __QUEUE_H__
#define __QUEUE_H__

//function declarations

struct Process {
    unsigned long time;
    int core;
    int operation;
    unsigned long long address;
};

struct Queue {
    int front, rear;
    int capacity;
    int size;
    struct Process *array;  // store Process
};
// might need them later
// struct Process frontElement(struct Queue *Q);
// struct Process rearElement(struct Queue *Q);
struct Queue *create_queue();
int size(struct Queue *Q);
int is_empty(struct Queue *Q);
int is_full(struct Queue *Q);
void enqueue(struct Queue *Q, struct Process data);
struct Process dequeue(struct Queue *Q);
void delete_queue(struct Queue *Q);
void print_queue(struct Queue *Q);

#endif