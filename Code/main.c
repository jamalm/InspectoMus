#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "log.h"
#include "MessageQueue.h"
#include "timer.h"
#include "Auditor.h"
#include "Backup.h"

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
	pid_t pid;
    
    char* QueueName = "/Daemon_Manager";
    //2 Phases
    //Active Phase
    //Dormant Phase

    //Create Message Queue
    mqd_t mq = CreateMasterQueue(QueueName);
    
    if(mq==-1)
    {
        printf("ERROR: Opening Message Queue Failed!\n Exiting...\n");
        LogErr("Queue", "Error Opening");
        exit(0);
    } 
    else 
    {
	    LogDaemon("Queue","Created");
        printf("Queue Created!\n");

    }

    //Create Timer.
    
    pid = fork();
    if(pid == 0)
    {
    	printf("\nCHILD\n");
        //start timer for child process
        if(StartTimer(QueueName)==0)
        {
            printf("\nTimer Closed successfully\n");
            exit(0);
        }
        else 
        {
            printf("Something went wrong with the timer!\n");
            exit(-1);
        }
    }else 
    {
    	printf("PARENT: child timer pid = %d\n", pid);
    }


    
    //Begin Dormant Phase
    //Start Process with Auditd
    pid = fork();
    if(pid == 0)
    {
    	RemoveRules();
    }
    else 
	{
		printf("PARENT: child removerule pid = %d\n", pid);
	}
    pid = fork();
    if(pid == 0)
    {
        AddRules();
    }
    else 
	{
		printf("PARENT: child addrule pid = %d\n", pid);
	}

    //Begin Active Phase
    //Lock out other users from directory
    //Create process to Create Backup
    //Create process to Create Transfer using changelog 
    
    if(pid != 0)
    {
    	while(1)
    	{
    	    //parent watches for messages incoming from the queue
		    char* message = Listen(mq);
		    printf("\nMessage received from queue: %s\n", message);

		    
		    //if message reads midnight, timer has hit 0, change to active phase
		    if(strncmp(message, "Midnight", sizeof(message)) == 0)
		    {
		    	//it is midnight, start active phase of locking, backing up and transfer
		    	LogDaemon("Phase", "Active");
		    	
		    	//lock directory
		    	pid = fork();
		    	if(pid == 0)
		    	{
		    		Lockup(QueueName);
		    		exit(-1);
		    	}
		    	else 
		    	{
		    		printf("PARENT: child lock pid = %d\n", pid);
		    	}
		    } else if(strncmp(message, "Locked", sizeof(message)) == 0)
		    {
		    	pid = fork();
		    	if(pid == 0)
		    	{
		    		Backup(QueueName);
		    		exit(-1);
		    	}
		    	else 
		    	{
		    		printf("PARENT: child backup pid = %d\n", pid);
		    	}
		    } else if(strncmp(message, "Backed Up", sizeof(message)) == 0)
		    {
		    	pid = fork();
		    	if(pid == 0)
		    	{
		    		Transfer(QueueName);
		    		exit(-1);
		    	}
		    	else 
		    	{
		    		printf("PARENT: child transfer pid = %d\n", pid);
		    	}
		    } else if(strncmp(message, "Transferred", sizeof(message)) == 0)
		    {
		    	pid = fork();
		    	if(pid == 0)
		    	{
		    		Unlock(QueueName);
		    		exit(-1);
		    	}
		    	else
		    	{
		    		printf("PARENT: child unlock pid = %d\n", pid);
		    	}
		    } else if(strncmp(message, "Unlocked", sizeof(message)) == 0)
		    {
		    	LogDaemon("Phase", "Dormant");
		    	pid = fork();
				if(pid == 0)
				{
					if(StartTimer(QueueName)==0)
					{
						printf("\nTimer Closed successfully\n");
						exit(0);
					}
					else 
					{
						printf("Something went wrong with the timer!\n");
						exit(-1);
					}
				}else 
				{
					printf("PARENT: child timer pid = %d\n", pid);
				}
		    }
    	}
    }
    CloseQueue(mq, QueueName);
    return 0;
}
