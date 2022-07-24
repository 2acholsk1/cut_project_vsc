#include "functions.h"
#include "functions.c"

#define READER_QUEUE_SIZE 13

//proc/stat/ file
FILE *procStatFile;

struct Queue* readerQueue;
struct Queue* analyzerQueue;

//Threads and etc.
pthread_t reader,analyzer,printer,watchdog,logger;

pthread_mutex_t readerLineBufMutex;
pthread_mutex_t analyzerMutex;

sem_t readerBufferFull, readerBufferEmpty;
sem_t analyzerBufferFull, analyzerBufferEmpty;

//Main functions

void* readData(void* arg)
{
    struct cpuData toSent[READER_QUEUE_SIZE];
    for(;;)
    {
        
        if((procStatFile=fopen("/proc/stat","r"))==NULL)
        {
            exit(EXIT_FAILURE);
        }
        
        char* lineBuf = NULL;
        size_t lineBufSize = 0;
        char checkCpuChar[3];
        int lineCounter = 0;
        ssize_t lineSize;

        
        while(1)
        {
            lineSize = getline(&lineBuf, &lineBufSize, procStatFile);
            if(strlen(lineBuf)>3)
            {
                strncpy(checkCpuChar, lineBuf, 3);
            }
            
            
            if(strcmp("cpu",checkCpuChar) != 0 )
            {
                break;
            }
            toSent[lineCounter] = cuttingCpuData(lineBuf);
            lineCounter++;


            //printf("%"PRIu64"\n", readerQueue->rear->key.user);
        }
        fclose(procStatFile);
        free(lineBuf);
        procStatFile=NULL;
        sem_wait(&readerBufferEmpty);
        pthread_mutex_lock(&readerLineBufMutex);
        for(int i = 0; i <READER_QUEUE_SIZE; i++)
        {
            enQueue(readerQueue, toSent[i]);
        }        
        pthread_mutex_unlock(&readerLineBufMutex);
        sem_post(&readerBufferFull);        

        sleep(1);
        
    }   
    return NULL;
}

void* analyzeData(void* arg)
{
    float cpuPercentage[READER_QUEUE_SIZE];
    struct cpuData toAnalyze[READER_QUEUE_SIZE],prevToAnalyze[READER_QUEUE_SIZE];
    bool notFirstRound = false;
    for(;;)
    {
        sem_wait(&readerBufferFull);
        pthread_mutex_lock(&readerLineBufMutex);
        for(int i = 0; i<READER_QUEUE_SIZE; i++)
        {
            toAnalyze[i] = readerQueue->front->key;
            deQueue(readerQueue);
            printf("show me %f",cpuPercentage[0]);
        }
                       
        pthread_mutex_unlock(&readerLineBufMutex);
        sem_post(&readerBufferEmpty);
        //printf("%"PRIu64"\n", readerQueue->rear->key.user);
        if(notFirstRound)
        {
            for(int i =0;i<READER_QUEUE_SIZE;i++)
            {
                cpuPercentage[i] = analyzeCpuData(toAnalyze[i],prevToAnalyze[i]);
            }
            
        }
        for(int i =0;i<READER_QUEUE_SIZE;i++)
        {
            prevToAnalyze[i] = toAnalyze[i];
        }
        notFirstRound = true;


    }
    return NULL;
}




void* loggingData(void* arg)
{

}


void clearAll()
{
    sem_destroy(&readerBufferEmpty);
    sem_destroy(&readerBufferFull);
    
    sem_destroy(&analyzerBufferEmpty);
    sem_destroy(&analyzerBufferFull);

    pthread_mutex_destroy(&readerLineBufMutex);
    pthread_mutex_destroy(&analyzerMutex);

    //clearing pthreaders
    pthread_join(reader,NULL);
    pthread_join(analyzer,NULL);
    pthread_join(printer,NULL);
    pthread_join(watchdog,NULL);
    pthread_join(logger,NULL);

    //destroyQueue(readerQueue);


    //closing file

}

int main()
{
    readerQueue = createQueue();
    analyzerQueue = createQueue();

    sem_init(&readerBufferEmpty, 0 ,1);
    sem_init(&readerBufferFull, 0 ,0);

    sem_init(&analyzerBufferEmpty,0,1);
    sem_init(&analyzerBufferFull,0,0);

    pthread_mutex_init(&readerLineBufMutex,NULL);

    pthread_create(&reader,NULL,&readData,NULL);  
    pthread_create(&analyzer,NULL,&analyzeData,NULL);
    clearAll();
    return EXIT_SUCCESS;
}