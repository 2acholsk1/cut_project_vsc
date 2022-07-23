#ifndef CUT_PROJECT_FUNCTIONS_H
#define CUT_PROJECT_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <semaphore.h>


typedef struct cpuData
{
    char* whichCpu;
    uint64_t user,nice,system,idle,iowait,irq,softirq,steal;
};

struct QNode
{
    struct cpuData key;
    struct QNode* next;
};

struct Queue
{
    struct QNode *front, *rear;
};

struct QNode* newNode(struct cpuData arg);
struct Queue* createQueue();
void enQueue(struct Queue* queue, struct cpuData arg);
void deQueue(struct Queue* queue);

//Queue functions
// bool isFull(struct Queue* queue);
// bool isEmpty(struct Queue* queue);
// struct Queue* cretaeQueue(unsigned size);
// void enQueue(struct Queue* queue, struct cpuData arg);
// struct cpuData deQueue(struct Queue* queue);
// struct cpuData front(struct Queue* queue);
// struct cpuData rear(struct Queue* queue);


struct cpuData cuttingCpuData(char* lineBuf);


//Function for cutting string






#endif
