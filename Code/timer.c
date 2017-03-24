#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include "log.h"
#include "timer.h"


//timer starts here
int StartTimer(char* Qname)
{
    //create time stuff
    time_t now;
    struct tm midnight;
    double seconds;
    //queue stuff here 
    mqd_t mq;
    char queueWBuffer[1024];

    //get current time
    time(&now);
    
    midnight = *localtime(&now);

    //set midnight to be 00:00am +1 day ahead of today
    midnight.tm_hour = 0;
    midnight.tm_min = 0;
    midnight.tm_sec = 0;
    midnight.tm_mday = midnight.tm_mday + 1;
    
    /*Open Message queue*/
    mq = mq_open(Qname, O_WRONLY);

	Log("Timer", "Starting Timer");
    //waiting on midnight
    while(1)
    {
        //check the time difference to start
        time(&now);
        seconds = difftime(mktime(&midnight), now);
        if(seconds==0)
        {
            char* midnightMsg = "Midnight";
            //if it is midnight, send a message to the queue for the daemon to read
            memset(queueWBuffer, 0, 1024);
            strcpy(queueWBuffer, midnightMsg);
            mq_send(mq, queueWBuffer, 1024, 0);
            mq_close(mq);
            //close timer after operation complete
            return 0;
        }
        sleep(1);
    }
    //should not get this far
    LogWarning("Timer", "outside loop");
    return 1;
}

//primarily used for processes that create file reports, allows for datestamp tags
char* GetDate()
{
	char* date = malloc(sizeof(char)*100);
	//create time stuff
    time_t now;
    struct tm* timeinfo;
    
    //get current time
    time(&now);
    timeinfo = localtime(&now);

	//format string to be our time format, not american
    strftime(date, sizeof(date)*2, "%e-%m-%G", timeinfo);
    
    return date;
}

