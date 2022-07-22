#include "functions.h"
#include "functions.c"



//proc/stat/ file
FILE *procStatFile;



//Queues
struct Queue* readerQueue;



//Threads
pthread_t reader,analyzer,printer,watchdog,logger;


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
        struct cpuData* toSent;

        lineSize = getline(&lineBuf, &lineBufSize, procStatFile);

            
        char checkCpuChar[3];
        strncpy(checkCpuChar, lineBuf, 3);
        
        if(strcmp("cpu",checkCpuChar) != 0 )
        {
            break;
        }
        cuttingCpuData(&toSent,lineBuf);
            
        
        
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