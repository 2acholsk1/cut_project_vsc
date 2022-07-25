#include "functions.h"
#include "functions.c"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <semaphore.h>

#define READER_AND_ANALYZER_QUEUE_SIZE 13

//proc/stat/ file
FILE *procStatFile;

struct Queue* readerQueue;
struct Queue* analyzerQueue;

//Threads and etc.
pthread_t reader, analyzer, printer;

pthread_mutex_t readerLineBufMutex;
pthread_mutex_t analyzerMutex;

sem_t readerBufferFull, readerBufferEmpty;
sem_t analyzerBufferFull, analyzerBufferEmpty;


bool notFirstRound = false;
//Main functions

void* readData()
{
    struct cpuData toSent[READER_AND_ANALYZER_QUEUE_SIZE];
    for(;;)
    {
        char* lineBuf = NULL;
        size_t lineBufSize = 0;
        char checkCpuChar[3];
        int lineCounter = 0;
        ssize_t lineSize;
          
        if((procStatFile=fopen("/proc/stat","r"))==NULL)
        {
            exit(EXIT_FAILURE);
        }
        while(1)
        {
            getline(&lineBuf, &lineBufSize, procStatFile);
            if(strlen(lineBuf)>3)
            {
                strncpy(checkCpuChar, lineBuf, 3);
            }
            
            
            if(strcmp("cpu",checkCpuChar) != 0 )
            {
                fclose(procStatFile);       
                procStatFile=NULL;
                free(lineBuf);
                break;
            }
            pthread_mutex_lock(&readerLineBufMutex);
            toSent[lineCounter] = cuttingCpuData(lineBuf);
            pthread_mutex_unlock(&readerLineBufMutex);
            lineCounter++;
        }
        sleep(1);
        sem_wait(&readerBufferEmpty);
        pthread_mutex_lock(&readerLineBufMutex);
        for(int it = 0; it<READER_AND_ANALYZER_QUEUE_SIZE; it++)
        {
            enQueue(readerQueue, toSent[it]);
        }
        pthread_mutex_unlock(&readerLineBufMutex);
        sem_post(&readerBufferFull);        

        
        
    }   
}

void* analyzeData()
{
    struct cpuData toAnalyze[READER_AND_ANALYZER_QUEUE_SIZE];
    struct cpuData prevToAnalyze[READER_AND_ANALYZER_QUEUE_SIZE];
    
    for(;;)
    {
        
        sem_wait(&readerBufferFull);
        pthread_mutex_lock(&readerLineBufMutex);
        for(int it = 0; it<READER_AND_ANALYZER_QUEUE_SIZE; it++)
        {
            toAnalyze[it] = readerQueue->front->key;
            deQueue(readerQueue);
        }
                        
        pthread_mutex_unlock(&readerLineBufMutex);
        sem_post(&readerBufferEmpty);
        
        sleep(1);
        if(notFirstRound)
        {
            for(int j =0;j<READER_AND_ANALYZER_QUEUE_SIZE;j++)
            {
                toAnalyze[j].cpuPercentage = analyzeCpuData(toAnalyze[j],prevToAnalyze[j]);
                printf("sending float: %f\n",toAnalyze[j].cpuPercentage);
            }
            
        }
        for(int it =0;it<READER_AND_ANALYZER_QUEUE_SIZE;it++)
        {
            prevToAnalyze[it] = toAnalyze[it];
        }        
        
        sem_wait(&analyzerBufferEmpty);
        pthread_mutex_lock(&analyzerMutex);
        if(notFirstRound)
        {
            for(int it = 0; it<READER_AND_ANALYZER_QUEUE_SIZE; it++)
            {
                enQueue(analyzerQueue,toAnalyze[it]);
            }  
        }
        pthread_mutex_unlock(&analyzerMutex);
        sem_post(&analyzerBufferFull);
        notFirstRound = true;
    }
}

// void* printData()
// {
//     float cpuPercentage[READER_AND_ANALYZER_QUEUE_SIZE];
//     struct cpuData floatsToPrint[READER_AND_ANALYZER_QUEUE_SIZE];
//     for(;;)
//     {
//         sem_wait(&analyzerBufferFull);
//         pthread_mutex_lock(&analyzerMutex);
//         // if(notFirstRound)
//         // {
//         //     floatsToPrint[i].cpuPercentage = analyzerQueue->front->key.cpuPercentage;
//         //     printf("show me %f" , floatsToPrint[i]);
//         //     deQueue(analyzerQueue);
//         // }  
//         pthread_mutex_unlock(&analyzerMutex);
//         sem_post(&analyzerBufferEmpty);
//         //printf("Show me her: %f",cpuPercentage[0]);
//         notFirstRound = true;
//     }
//     return NULL;
// }


// void* loggingData(void* arg)
// {

// }


void clearAll()
{

    pthread_join(reader,NULL);
    pthread_join(analyzer,NULL);
    pthread_join(printer,NULL);

    sem_destroy(&readerBufferEmpty);
    sem_destroy(&readerBufferFull);
    
    sem_destroy(&analyzerBufferEmpty);
    sem_destroy(&analyzerBufferFull);

    pthread_mutex_destroy(&readerLineBufMutex);
    pthread_mutex_destroy(&analyzerMutex);

}

int main()
{
    
    readerQueue = createQueue();
    analyzerQueue = createQueue();

    sem_init(&readerBufferEmpty,0 ,READER_AND_ANALYZER_QUEUE_SIZE);
    sem_init(&readerBufferFull, 0 ,0);

    pthread_mutex_init(&readerLineBufMutex,NULL);

    
    sem_init(&analyzerBufferEmpty,0,READER_AND_ANALYZER_QUEUE_SIZE);
    sem_init(&analyzerBufferFull,0,0);

    pthread_mutex_init(&analyzerMutex,NULL);
    
    

    pthread_create(&reader,NULL,&readData,NULL);  
    pthread_create(&analyzer,NULL,&analyzeData,NULL);
    //pthread_create(&printer,NULL,&printData,NULL);



    clearAll();
    return EXIT_SUCCESS;
}