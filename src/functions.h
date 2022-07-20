#ifndef CUT_PROJECT_FUNCTIONS_H
#define CUT_PROJECT_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct Queue
{
    int front;
    int rear;
    int size;
    unsigned capacity;
    int* array;
};

struct Queue* createQueue(unsigned capacity);
void enQueue(struct Queue* queue, int arg);
int deQueue(struct Queue* queue);
int front(struct Queue* queue);
int rear(struct Queue* queue);

 





#endif
