#include "functions.h"



//creating a queue
// struct Queue* cretaeQueue(unsigned _capacity)
// {
//     struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
//     queue->capacity = _capacity;
//     queue->front = queue->size = 0;
//     queue->rear = _capacity - 1;
//     queue->array = (struct cpuData*)malloc(queue->capacity * sizeof(struct cpuData));
//     return queue;
// };

// bool isFull(struct Queue* queue)
// {
//     return queue->size == queue->capacity;
// }

// bool isEmpty(struct Queue* queue)
// {
//     return queue->size == 0;
// }

// void enQueue(struct Queue* _queue, struct cpuData arg)
// {
//     struct Queue* queue= cretaeQueue(1000);
//     if(isFull(queue))
//     {
//         return;
//     }
    
//     queue->array[queue->rear] = arg;
//     queue->size = queue->size + 1;
//     queue->rear = (queue->rear + 1) % queue->capacity;
// }

// struct cpuData deQueue(struct Queue* queue)
// {
//     if(isEmpty(queue))
//     {
//         return;
//     }
//     struct cpuData item = queue->array[queue->front];
//     queue->front = (queue->front + 1) % queue->capacity;
//     queue->size = queue->size - 1;
//     return item;
// };

// struct cpuData front(struct Queue* queue)
// {
//     if(isEmpty(queue))
//     {
//         return queue->array[0];
//     }
//     return queue->array[queue->front];
// };

// struct cpuData rear(struct Queue* queue)
// {
//     if(isEmpty(queue))
//     {
//         return queue->array[0];
//     }
//     return queue->array[queue->rear];
// };

struct QNode* newNode(struct cpuData arg)
{
    struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->key = arg;
    temp->next = 0;
    return temp;
}



struct Queue* createQueue()
{
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = 0;
    return q;
}



void enQueue(struct Queue* queue, struct cpuData key)
{
    struct QNode* temp = newNode(key);
  
    if (queue->rear == 0) {
        queue->front = queue->rear = temp;
        return;
    }
    queue->rear->next = temp;
    queue->rear = temp;
}



void deQueue(struct Queue* queue)
{
    if (queue->front == 0)
        return;

    struct QNode* temp = queue->front;
  
   queue->front = queue->front->next;
  
    if (queue->front == 0)
        queue->rear = 0;
  
    free(temp);
}



struct cpuData cuttingCpuData(char* lineBuf)
{ 
    struct cpuData* arg;
    char* out = strtok(lineBuf, " ");
    int counter = 0;
    uint64_t toConvert;
    while (counter<9)
    {
        if(counter>0)
        {
            toConvert = atoll(out);
        }
        switch (counter)
        {
        case 0:
            arg->whichCpu = out;
            break;
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
        out = strtok (0, " ");        
    }
    return *arg;
}