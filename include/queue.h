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
struct Queue *createQueue();
int size(struct Queue *Q);
int isEmpty(struct Queue *Q);
int isFull(struct Queue *Q);
void enqueue(struct Queue *Q, struct Process data);
struct Process dequeue(struct Queue *Q);
void deleteQueue(struct Queue *Q);
void printQueue(struct Queue *Q);

#endif