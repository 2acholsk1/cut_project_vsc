#include "functions.h"
#include "functions.c"



//proc/stat/ file
FILE *procStatFile;

struct Queue* readerQueue;

//Threads and etc.
pthread_t reader,analyzer,printer,watchdog,logger;

pthread_mutex_t readerLineBufMutex;

sem_t readerBufferFull, readerBufferEmpty;

//Main functions

void* readData(void* arg)
{
    
    for(;;)
    {
        
        if((procStatFile=fopen("/proc/stat","r"))==NULL)
        {
            exit(EXIT_FAILURE);
        }
        
        char* lineBuf = NULL;
        size_t lineBufSize = 0;
        struct cpuData toSent;

        char checkCpuChar[3];
        while(1)
        {
            getline(&lineBuf, &lineBufSize, procStatFile);
            if(strlen(lineBuf)>3)
            {
                strncpy(checkCpuChar, lineBuf, 3);
            }
            
            
            if(strcmp("cpu",checkCpuChar) != 0 )
            {
                break;
            }
            toSent = cuttingCpuData(lineBuf);
            sem_wait(&readerBufferEmpty);
            pthread_mutex_lock(&readerLineBufMutex);
            enQueue(readerQueue, toSent);
            pthread_mutex_unlock(&readerLineBufMutex);
            sem_post(&readerBufferFull);
            printf("%"PRIu64"\n", readerQueue->rear->key.user);
        }
        fclose(procStatFile);
        free(lineBuf);
        procStatFile=NULL;
        
        
    }   
}

void* analyzeData(void* arg)
{
    struct cpuData data;

    for(;;)
    {
        sem_wait(&readerBufferFull);
        pthread_mutex_lock(&readerLineBufMutex);
        data = readerQueue->front->key;
        printf("%"PRIu64"\n", readerQueue->rear->key.user);

        sem_post(&readerBufferEmpty);
        pthread_mutex_unlock(&readerLineBufMutex);
    }

}

void* loggingData(void* arg)
{

}


void clearAll()
{
    sem_destroy(&readerBufferEmpty);
    sem_destroy(&readerBufferFull);


    pthread_mutex_destroy(&readerLineBufMutex);

    //clearing pthreaders
    pthread_join(reader,NULL);
    pthread_join(analyzer,NULL);
    pthread_join(printer,NULL);
    pthread_join(watchdog,NULL);
    pthread_join(logger,NULL);


    //closing file

}

int main()
{
    readerQueue = createQueue();

    sem_init(&readerBufferEmpty, 0 ,15);
    sem_init(&readerBufferFull, 0 ,0);

    pthread_mutex_init(&readerLineBufMutex,NULL);

    pthread_create(&reader,NULL,&readData,NULL);  
    pthread_create(&analyzer,NULL,&analyzeData,NULL);
    clearAll();
    return EXIT_SUCCESS;
}