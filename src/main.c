#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>




//proc/stat/ file
FILE *procStatFile;



//Variables



//Threads
pthread_t reader,analyzer,printer,watchdog,logger;


//Main functions

void* read_data(void* args)
{
    if((procStatFile=fopen("proc/stat","r"))==NULL)
    {
        exit(EXIT_FAILURE);
    }

    char* whichLine = NULL;
    size_t length = 0;
    ssize_t line = 0;

    while((line=getline(&whichLine,&length,procStatFile))!= -1)
    {
        
        
    } 
    
}


void clearAll()
{
    //clearing pthreaders
    pthread_join(reader,NULL);
    pthread_join(analyzer,NULL);
    pthread_join(printer,NULL);
    pthread_join(watchdog,NULL);
    pthread_join(logger,NULL);
}

int main()
{
    pthread_create(&reader,NULL,&read_data,NULL);
    clearAll();
    return EXIT_SUCCESS;
}