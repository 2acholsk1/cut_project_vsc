#ifndef CUT_PROJECT_FUNCTIONS_H
#define CUT_PROJECT_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


struct cpuData
{
    char* whichCpu;
    uint64_t user;
    uint64_t nice;
    uint64_t system;
    uint64_t idle;
    uint64_t iowait;
    uint64_t irq;
    uint64_t softirq;
    uint64_t steal;
}cpuData;

struct Queue
{
    uint64_t front;
    uint64_t rear;
    uint64_t size;
    unsigned capacity;
    struct cpuData* array;
}Queue;


//Queue functions
bool isFull(struct Queue* queue);
bool isEmpty(struct Queue* queue);
struct Queue* cretaeQueue(unsigned size);
void enQueue(struct Queue* queue, struct cpuData arg);
struct cpuData deQueue(struct Queue* queue);
struct cpuData front(struct Queue* queue);
struct cpuData rear(struct Queue* queue);


struct cpuData cuttingCpuData(char* lineBuf);


//Function for cutting string






#endif
