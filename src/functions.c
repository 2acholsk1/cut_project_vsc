#include "functions.h"



//creating a queue
struct Queue* createQueue(unsigned _capacity)
{
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->capacity = _capacity;
    queue->front = queue->size = 0;
    queue->rear = _capacity-1;
    queue->array = (int)malloc(queue->capacity * sizeof(int));
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
        return queue->array[0];
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = arg;
    queue->size = queue->size + 1;
}

int deQueue(struct Queue* queue)
{
    if(isEmpty(queue))
    {
        return queue->array[0];
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
    while (out != NULL)
    {
        counter++;
        switch (counter)
        {
        case 1:
            toConvert = strtoull(out, NULL , 0);
            arg->user = toConvert;
            out = strtok (NULL, " ");
            break;
        case 2:
            toConvert = strtoull(out, NULL , 0);
            arg->nice = toConvert;
            out = strtok (NULL, " ");
            break;
        case 3:
            toConvert = strtoull(out, NULL , 0);
            arg->system = toConvert;
            out = strtok (NULL, " ");
            break;
        case 4:
            toConvert = strtoull(out, NULL , 0);
            arg->idle = toConvert;
            out = strtok (NULL, " ");
            break;
        case 5:
            toConvert = strtoull(out, NULL , 0);
            arg->iowait = toConvert;
            out = strtok (NULL, " ");
            break;
        case 6:
            toConvert = strtoull(out, NULL , 0);
            arg->irq = toConvert;
            out = strtok (NULL, " ");
            break;
        case 7:
            toConvert = strtoull(out, NULL , 0);
            arg->softirq = toConvert;
            out = strtok (NULL, " ");
            break;
        case 8:
            toConvert = strtoull(out, NULL , 0);
            arg->steal = toConvert;
            out = strtok (NULL, " ");
            break;
        
        default:
            break;
        }
    }
    
};