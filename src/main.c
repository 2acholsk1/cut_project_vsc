#include "functions.h"




//proc/stat/ file
FILE *procStatFile;



//Queues




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
        int lineCount = 0;
        ssize_t lineSize = 0;

        lineSize = getline(&lineBuf, &lineBufSize, procStatFile);

        while (1)
        {
            lineCount++;

            /*printf("line[%06d]: chars=%06zd, buf size=%06zu, contents: %s", lineCount,
                lineSize, lineBufSize, lineBuf); */ 

            lineSize = getline(&lineBuf, &lineBufSize, procStatFile);
            if(strncmp(lineBuf,"cpu",3) != 1)
            {
                break;
            }
            
        }

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