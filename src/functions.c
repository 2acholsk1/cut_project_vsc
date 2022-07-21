#include "functions.h"


//creating a queue
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity-1;
    queue->array = (int*)malloc(queue->capacity * sizeof(int));
    return queue;
};

int isFull(struct Queue* queue)
{
    return (queue->size == queue->capacity);
}

int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}

void enQueue(struct Queue* queue, struct cpuData arg)
{
    if(isFull(queue))
    {
        return queue->capacity;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = arg;
    queue->size = queue->size + 1;
}

struct cpuData deQueue(struct Queue* queue)
{
    if(isEmpty(queue))
    {
        return queue->array[0];
    }
    struct cpuData item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

struct cpuData front(struct Queue* queue)
{
    if(isEmpty(queue))
    {
        return queue->array[0];
    }
    return queue->array[queue->front];
}

struct cpuData rear(struct Queue* queue)
{
    if(isEmpty(queue))
    {
        return queue->array[0];
    }
    return queue->array[queue->rear];
}

struct cpuData* createCpuData(struct cpuData* arg,char* lineBuf)
{
    char* array = malloc(sizeof(lineBuf));
    
};