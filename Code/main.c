#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "log.h"
#include "MessageQueue.h"
#include "timer.h"

/*
Author: Jamal Mahmoud - C13730921

Description:
Daemon Script that communicates through a message queue to it's child processes and receives outputs/errors from child processes
1. Create Message Queue, Logger, Timer.
2. Wait for Timer to return true to begin Active Phase.
Active Phase
1. Lock folders involved in backup/transfer.
2. Wait for response from backup and begin transfer of tagged files.
3. Wait for response from transfer process and begin dormant phase.
4. Log all changes and responses from processes
Dormant Phase
1. Watch Directories using Auditd and log any changes committed.
2. Wait for Executive order or Timer to begin Active Phase.
*/

int main()
{

    
    char* QueueName = "/Daemon_Manager";
    //2 Phases
    //Active Phase
    //Dormant Phase

    //Create Message Queue
    mqd_t mq = CreateMasterQueue(QueueName);
    
    if(mq==-1)
    {
        printf("ERROR: Opening Message Queue Failed!\n Exiting...\n");
        exit(0);
    } 
    else 
    {
        printf("Queue Created!\n");
        //start listening

        //if something happens, close queue
        //CloseQueue(mq, QueueName);

    }
    
    //Create Timer.
    pid_t pid = fork();
    if(pid == 0)
    {
        //start timer for child process
        if(StartTimer(QueueName)==0)
        {
            printf("Timer Closed successfully\n");
            exit(0);
        }
        else 
        {
            printf("Something went wrong with the timer!\n");
            exit(-1);
        }
    }

    if(pid != 0)
    {
        //parent watches for messages incoming from the queue
        char* message = Listen(mq);
        printf("Message received from timer: %s", message);
    }

    //Begin Dormant Phase
    //Start Process with Auditd
    //Begin Active Phase
    //Lock out other users from directory
    //Create process to Create Backup
    //Create process to Create Transfer using changelog 

    CloseQueue(mq, QueueName);
    return 0;
}
