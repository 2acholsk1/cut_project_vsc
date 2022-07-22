#include "functions.h"
#include "functions.c"


#include <inttypes.h>
//proc/stat/ file
FILE *procStatFile;



//Queues
struct Queue* readerQueue;



//Threads and etc.
pthread_t reader,analyzer,printer,watchdog,logger;

pthread_mutex_t readerLineBufMutex;


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
        ssize_t lineSize = 0;

        lineSize = getline(&lineBuf, &lineBufSize, procStatFile);
           
        char checkCpuChar[3];
        strncpy(checkCpuChar, lineBuf, 3);
        
        if(strcmp("cpu",checkCpuChar) != 0 )
        {
            break;
        }

        pthread_mutex_lock(&readerLineBufMutex);
        struct cpuData toSent = cuttingCpuData(lineBuf);
        enQueue(readerQueue,toSent);
        pthread_mutex_unlock(&readerLineBufMutex);    
        
        free(lineBuf);
        lineBuf = NULL;

        
    }   
}

void* loggingData(void* arg)
{

}


void clearAll()
{
    //clearing pthreaders
    pthread_join(reader,NULL);
    pthread_join(analyzer,NULL);
    pthread_join(printer,NULL);
    pthread_join(watchdog,NULL);
    pthread_join(logger,NULL);


    //closing file
    fclose(procStatFile);
    procStatFile=NULL;
}

int main()
{
    pthread_create(&reader,NULL,&readData,NULL);
    clearAll();
    return EXIT_SUCCESS;
}