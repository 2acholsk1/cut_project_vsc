#include "functions.h"


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
  
    if (queue->rear == NULL) 
    {
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

void destroyQueue(struct Queue* queue)
{
    while(queue->front != 0)
    {
        deQueue(queue);
    }
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
        out = strtok (NULL, " ");        
    }
    return *arg;
}


float analyzeCpuData(struct cpuData arg,struct cpuData prevArg)
{
    uint64_t prevIdle = prevArg.idle + prevArg.iowait;
    uint64_t idle = arg.idle + arg.iowait;

    uint64_t prevNonIdle = prevArg.user + prevArg.nice + prevArg.system + prevArg.irq + prevArg.softirq + prevArg.steal;
    uint64_t nonIdle = arg.user + arg.nice + arg.system + arg.irq + arg.softirq + arg.steal;

    uint64_t prevTotal = prevIdle + prevNonIdle;
    uint64_t total = idle + nonIdle;

    uint64_t totald = total - prevTotal;
    uint64_t idled = idle - prevIdle;

    return (float)(totald-idled)*100.f/totald;
}