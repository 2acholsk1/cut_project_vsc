#include "functions.h"
// #include "functions.c"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <semaphore.h>
#include <signal.h>

#define READER_AND_ANALYZER_QUEUE_SIZE 13
#define MAX_LENGTH_OF_LOG_MESSAGE 24
#define WATCHTIME 2
#define REFRESH_TIME 1

volatile sig_atomic_t done = 0;

//proc/stat/ file
FILE *procStatFile;
FILE *logFile;

struct Queue* readerQueue;
struct Queue* analyzerQueue;


//Threads and etc.
pthread_t reader, analyzer, printer, watchdog, logger;

//Mutexes
pthread_mutex_t readerLineBufMutex;
pthread_mutex_t analyzerMutex;
pthread_mutex_t debugMutex;

//Semaphores
sem_t readerBufferFull, readerBufferEmpty;
sem_t analyzerBufferFull, analyzerBufferEmpty;
sem_t loggerBufferFull, loggerBufferEmpty;

//Variables
bool notFirstRound = false;
bool programNotWorks = false;
char logMessage[MAX_LENGTH_OF_LOG_MESSAGE] = "";
int logIterator = 1;
int counter = 0;

//Main functions
void* readData();
void* analyzeData();
void* printData();
void* checkingThreads(void* arg);
void* loggingData(void* arg);
void term(int signum);
void clearAll();



void* readData()
{
    struct cpuData toSent[READER_AND_ANALYZER_QUEUE_SIZE];
    struct timespec threadWorkingTime;
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
        else
        {
            sem_wait(&loggerBufferEmpty);
            pthread_mutex_lock(&debugMutex);
            strcpy(logMessage,"Reading proc/stat file");
            pthread_mutex_unlock(&debugMutex);
            sem_post(&loggerBufferFull);

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
            
            timespec_get(&threadWorkingTime, TIME_UTC);
            threadWorkingTime.tv_sec += WATCHTIME;
            if(sem_timedwait(&readerBufferEmpty, &threadWorkingTime) < 0)
            {
                programNotWorks = true;
            }
            pthread_mutex_lock(&readerLineBufMutex);
            for(int it = 0; it<READER_AND_ANALYZER_QUEUE_SIZE; it++)
            {
                enQueue(readerQueue, toSent[it]);
            }
            pthread_mutex_unlock(&readerLineBufMutex);
            sem_post(&readerBufferFull);
            
            sem_wait(&loggerBufferEmpty);
            pthread_mutex_lock(&debugMutex);
            strcpy(logMessage,"Data sent to analyze");
            pthread_mutex_unlock(&debugMutex);
            sem_post(&loggerBufferFull);

            sleep(REFRESH_TIME);
        }
    } 
    return NULL;  
}

void* analyzeData()
{
    struct cpuData toAnalyze[READER_AND_ANALYZER_QUEUE_SIZE];
    struct cpuData prevToAnalyze[READER_AND_ANALYZER_QUEUE_SIZE];
    struct timespec threadWorkingTime;
    for(;;)
    {
        
        timespec_get(&threadWorkingTime, TIME_UTC);
        threadWorkingTime.tv_sec += WATCHTIME;
        if(sem_timedwait(&readerBufferFull, &threadWorkingTime) < 0)
        {
            programNotWorks = true;
        }
        pthread_mutex_lock(&readerLineBufMutex);
        for(int it = 0; it<READER_AND_ANALYZER_QUEUE_SIZE; it++)
        {
            toAnalyze[it] = readerQueue->front->key;
            deQueue(readerQueue);
        }
                        
        pthread_mutex_unlock(&readerLineBufMutex);
        sem_post(&readerBufferEmpty);
        
        sleep(REFRESH_TIME);

        if(notFirstRound)
        {
            for(int j =0;j<READER_AND_ANALYZER_QUEUE_SIZE;j++)
            {
                toAnalyze[j].cpuPercentage = analyzeCpuData(toAnalyze[j],prevToAnalyze[j]);
            }
            
        }
        for(int it =0;it<READER_AND_ANALYZER_QUEUE_SIZE;it++)
        {
            prevToAnalyze[it] = toAnalyze[it];
        }      
        
        if(counter==1)
        {
            notFirstRound = true;
        }  
        counter++;

        timespec_get(&threadWorkingTime, TIME_UTC);
        threadWorkingTime.tv_sec += WATCHTIME;
        if(sem_timedwait(&analyzerBufferEmpty, &threadWorkingTime) < 0)
        {
            programNotWorks = true;
        }
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

        sem_wait(&loggerBufferEmpty);
        pthread_mutex_lock(&debugMutex);
        strcpy(logMessage,"Data sent to print");
        pthread_mutex_unlock(&debugMutex);
        sem_post(&loggerBufferFull);
    }
    return NULL;
}

void* printData()
{
    struct cpuData floatsToPrint[READER_AND_ANALYZER_QUEUE_SIZE];
    struct timespec threadWorkingTime;
    for(;;)
    {
        timespec_get(&threadWorkingTime, TIME_UTC);
        threadWorkingTime.tv_sec += WATCHTIME;
        if(sem_timedwait(&analyzerBufferFull, &threadWorkingTime) < 0)
        {
            programNotWorks = true;
        }
        pthread_mutex_lock(&analyzerMutex);
        if(notFirstRound)
        {
            for(int it = 0; it<READER_AND_ANALYZER_QUEUE_SIZE;it++)
            {
                floatsToPrint[it].cpuPercentage = analyzerQueue->front->key.cpuPercentage;    
                deQueue(analyzerQueue);
            }

        }  
        pthread_mutex_unlock(&analyzerMutex);
        sem_post(&analyzerBufferEmpty);

        if(notFirstRound)
        {
            for(int it = 0;it<READER_AND_ANALYZER_QUEUE_SIZE;it++)
            {
                if(it==0)
                {
                    printf("cpu -> %.1f%%\n",floatsToPrint[it].cpuPercentage);

                }
                else
                {
                    printf("cpu%i -> %.1f%%\n" ,it-1, floatsToPrint[it].cpuPercentage);
                }
            }
            
        }
        
        
        sem_wait(&loggerBufferEmpty);
        pthread_mutex_lock(&debugMutex);
        strcpy(logMessage,"Data printed");
        pthread_mutex_unlock(&debugMutex);
        sem_post(&loggerBufferFull);

        sleep(REFRESH_TIME);
        system("clear");
    }
    return NULL;
}


void* checkingThreads(void* arg)
{
    for(;;)
    {
        sleep(WATCHTIME);
        if(programNotWorks)
        {
        printf("Problems with threads. Closing the programme. ");
        clearAll();
        exit(EXIT_FAILURE);
        }
    }
    return NULL;
}



void* loggingData(void* arg)
{
    
    for(;;)
    {
        sem_wait(&loggerBufferFull);
        pthread_mutex_lock(&debugMutex);
        if((logFile=fopen("debugLogs","a")) == NULL)
        {
            clearAll();
            exit(EXIT_FAILURE);
        }
        else
        {
            fprintf(logFile,"Message number %i: %s\n",logIterator, logMessage);
            fclose(logFile);
        }
        logIterator++;
        pthread_mutex_unlock(&debugMutex);
        sem_post(&loggerBufferEmpty);
    }
    return NULL;
}



void term(int signum)
{
    done = 1;
}



void clearAll()
{
    pthread_join(reader,NULL);
    pthread_join(analyzer,NULL);
    pthread_join(printer,NULL);
    pthread_join(watchdog,NULL);
    pthread_join(logger,NULL);

    sem_destroy(&readerBufferEmpty);
    sem_destroy(&readerBufferFull);
    
    sem_destroy(&analyzerBufferEmpty);
    sem_destroy(&analyzerBufferFull);

    sem_destroy(&loggerBufferEmpty);
    sem_destroy(&loggerBufferFull);

    pthread_mutex_destroy(&readerLineBufMutex);
    pthread_mutex_destroy(&analyzerMutex);
    pthread_mutex_destroy(&debugMutex);
}

int main()
{
    struct sigaction action;
    memset(&action, 0 ,sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM,&action,NULL);

    readerQueue = createQueue();
    analyzerQueue = createQueue();

    sem_init(&readerBufferEmpty,0 ,READER_AND_ANALYZER_QUEUE_SIZE);
    sem_init(&readerBufferFull, 0 ,0);

    pthread_mutex_init(&readerLineBufMutex,NULL);
    
    sem_init(&analyzerBufferEmpty,0,READER_AND_ANALYZER_QUEUE_SIZE);
    sem_init(&analyzerBufferFull,0,0);

    pthread_mutex_init(&analyzerMutex,NULL);

    sem_init(&loggerBufferEmpty,0,1);
    sem_init(&loggerBufferFull,0,0);

    pthread_mutex_init(&debugMutex,NULL);

    pthread_create(&reader,NULL,&readData,NULL);  
    pthread_create(&analyzer,NULL,&analyzeData,NULL);
    pthread_create(&printer,NULL,&printData,NULL);
    pthread_create(&watchdog,NULL,&checkingThreads,NULL);
    pthread_create(&logger,NULL,&loggingData,NULL);

    clearAll();
    return EXIT_SUCCESS;
}