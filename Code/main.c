#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
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

Side Notes:
For those that don't know
Daemon: Runs around in the shadows(background) doing devilish deeds
*/

int main()
{
	//create daemon process
	int daemonPID = fork();
	pid_t timerPID;
	if(daemonPID > 0)
	{
		//parent
		Log("Startup", "Daemon Initialised");
		exit(EXIT_SUCCESS);	
	} else if(daemonPID == 0)
	{
		//child
		while(getppid() != 1)
		{
			LogDaemon("Init", "Waiting for parent to terminate");
			sleep(1);
		}
		umask(0);
		if(setsid() < 0)
		{
			LogErrDaemon("Set SID", "Failure");
			exit(EXIT_FAILURE);
		} else 
		{
			LogDaemon("Init sid", "Success");
		}
		
		if(chdir("/") < 0)
		{
			LogErrDaemon("ChangeDir", "Failure");
			exit(EXIT_FAILURE);
		} else 
		{
			LogDaemon("Init chdir", "Success");
		}
		//release IO Conns
		int fd;
		for(fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd --)
		{
			close(fd);
		} 
		signal(SIGCHLD, SIG_IGN);
		LogDaemon("Init", "Completed");
	}
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
    }

    //Create Timer.
    
    timerPID = fork();
    if(timerPID == 0)
    {
        //start timer for child process
        if(StartTimer(QueueName)== 0)
        {
            Log("Timer", "Successfully Closed");
            exit(EXIT_SUCCESS);
        }
        else 
        {
        	LogErr("Timer", "Failed to close successfully");
            exit(EXIT_FAILURE);
        }
    }


    
    //Begin Dormant Phase
    //Start Process with Auditd
    pid = fork();
    if(pid == 0)
    {
    	RemoveRules();
    	exit(1);
    }
    
    pid = fork();
    if(pid == 0)
    {
    	sleep(1);
        AddRules();
        exit(1);
    }
    

    //Begin Active Phase
    //Lock out other users from directory
    //Create process to Create Backup
    //Create process to Create Transfer using changelog 
    
    if(pid != 0)
    {
    	LogDaemon("Phase", "Dormant");
    	AuditReport();
    	while(1)
    	{
    		
    	    //parent watches for messages incoming from the queue
		    char* message = Listen(mq);
		    LogDaemon("Message From Queue", message);

		    if(strncmp(message, "Shutdown", sizeof(message)) == 0)
		    {
		    	LogDaemon("Status", "Shutting Down");
		    	kill(timerPID, SIGKILL);
		    	LogDaemon("Timer", "Killing timer");
		    	break;
		    }
		    //if message reads midnight, timer has hit 0, change to active phase
		    if(strncmp(message, "Midnight", sizeof(message)) == 0)
		    {
		    	//it is midnight, start active phase of locking, backing up and transfer
		    	LogDaemon("Phase", "Active");
		    	kill(timerPID, SIGKILL);
		    	LogDaemon("Timer", "Killing timer");
		    	//lock directory
		    	pid = fork();
		    	if(pid == 0)
		    	{
		    		Lockup(QueueName);
		    		exit(-1);
		    	}
		    } else if(strncmp(message, "Locked", sizeof(message)) == 0)
		    {
		    	pid = fork();
		    	if(pid == 0)
		    	{
		    		Backup(QueueName);
		    		exit(-1);
		    	}
		    } else if(strncmp(message, "Backed Up", sizeof(message)) == 0)
		    {
		    	pid = fork();
		    	if(pid == 0)
		    	{
		    		Transfer(QueueName);
		    		exit(1);
		    	}
		    } else if(strncmp(message, "Transferred", sizeof(message)) == 0)
		    {
		    	pid = fork();
		    	if(pid == 0)
		    	{
		    		Unlock(QueueName);
		    		exit(1);
		    	}
		    } else if(strncmp(message, "Unlocked", sizeof(message)) == 0)
		    {
		    	LogDaemon("Phase", "Dormant");
		    	timerPID = fork();
				if(timerPID == 0)
				{
					if(StartTimer(QueueName)==0)
					{

						exit(0);
					}
					else 
					{

						exit(1);
					}
				}
		    }
    	}
    }
    LogDaemon("Shutdown", "Closing Queue and shutting down");
    CloseQueue(mq, QueueName);
    return 0;
}
