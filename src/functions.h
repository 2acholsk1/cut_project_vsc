#ifndef CUT_PROJECT_FUNCTIONS_H
#define CUT_PROJECT_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

struct Queue
{
    int front;
    int rear;
    int size;
    unsigned capacity;
    int* array;
};
struct cpuData
{
    uint64_t user;
    uint64_t nice;
    uint64_t system;
    uint64_t idle;
    uint64_t iowait;
    uint64_t irq;
    uint64_t softirq;
    uint64_t steal;
};


//Queue functions
int isFull(struct Queue* queue);
int isEmpty(struct Queue* queue);
struct Queue* createQueue(unsigned capacity);
void enQueue(struct Queue* queue, int arg);
int deQueue(struct Queue* queue);
int front(struct Queue* queue);
int rear(struct Queue* queue);



void cuttingCpuData(struct cpuData* arg,char* lineBuf);


//Function for cutting string






#endif
