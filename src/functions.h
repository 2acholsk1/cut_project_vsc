#ifndef CUT_PROJECT_FUNCTIONS_H
#define CUT_PROJECT_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <semaphore.h>



typedef struct cpuData
{
    char* whichCpu;
    float cpuPercentage;
    uint64_t user,nice,system,idle,iowait,irq,softirq,steal;
}cpuData;

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
void destroyQueue(struct Queue* queue);

//Function for cutting string
struct cpuData cuttingCpuData(char* lineBuf);
//Function for receiving data and analyze it
float analyzeCpuData(struct cpuData arg,struct cpuData prevarg);







#endif
