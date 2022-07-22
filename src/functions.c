#include "functions.h"



//creating a queue
struct Queue* createQueue(unsigned _capacity)
{
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = _capacity;
    queue->front = queue->size = 0;
    queue->rear = _capacity-1;
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

void enQueue(struct Queue* queue, int arg)
{
    if(isFull(queue))
    {
        return EXIT_FAILURE;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = arg;
    queue->size = queue->size + 1;
}

int deQueue(struct Queue* queue)
{
    if(isEmpty(queue))
    {
        return queue->capacity;
    }
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
};

int front(struct Queue* queue)
{
    if(isEmpty(queue))
    {
        return queue->array[0];
    }
    return queue->array[queue->front];
};

int rear(struct Queue* queue)
{
    if(isEmpty(queue))
    {
        return queue->array[0];
    }
    return queue->array[queue->rear];
};

void cuttingCpuData(struct cpuData* arg,char* lineBuf)
{
    //char* array = malloc(sizeof(lineBuf)); 
    char* out = strtok(lineBuf, " ");
    int counter = 0;
    uint64_t toConvert;
    while (counter<9)
    {
        toConvert = atoll(out);
        switch (counter)
        {
        case 1:
            arg->user = toConvert;
            break;
        case 2:
            arg->nice = toConvert;
            break;
        case 3:
            arg->system = toConvert;
            break;
        case 4:
            arg->idle = toConvert;
            break;
        case 5:
            arg->iowait = toConvert;
            break;
        case 6:
            arg->irq = toConvert;
            break;
        case 7:
            arg->softirq = toConvert;
            break;
        case 8:
            arg->steal = toConvert;
            break;
        default:
            break;
        }
        counter++;
        out = strtok (NULL, " ");
    }
    
};