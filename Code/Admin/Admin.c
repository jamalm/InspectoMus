#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <mqueue.h>

#define ERRMSG "Invalid args\nUsage: Admin [OPTIONS]{shutdown | backup | startup} [PATH] {Enter path to daemon}\n"

void AdminLog(char *message);
void AdminErrLog(char *message);

int main(int argc, char* argv[])
{
	if(argc < 2 || argc > 3)
	{
		printf(ERRMSG);
		AdminErrLog("Invalid Input");
		exit(EXIT_FAILURE);
	}

	printf("Welcome to Web Version Controller Daemon Access\n");
	printf("For the purposes of security and liability, all actions are logged in this program\n");
	printf("Processing Request: %s\n", argv[1]);
	
	//queue stuff here 
    mqd_t mq;
    char queueWBuffer[1024];
    mq = mq_open("/Daemon_Manager", O_WRONLY);
    
    if(argc == 2)
    {
		if(strcmp(argv[1], "shutdown") == 0)
		{
			//send shutdown signal
			 memset(queueWBuffer, 0, 1024);
		    strcpy(queueWBuffer, "Shutdown");
		    mq_send(mq, queueWBuffer, 1024, 0);
		    mq_close(mq);
		    printf("\nOperation Successful\nHave a nice day!\n");

		    //log action
		    AdminLog("Shutdown Daemon");

		} 
		else if(strcmp(argv[1], "backup") == 0)
		{
			//send backup signal
			memset(queueWBuffer, 0, 1024);
		    strcpy(queueWBuffer, "Midnight");
		    mq_send(mq, queueWBuffer, 1024, 0);
		    mq_close(mq);
		    printf("\nOperation Successful\nHave a nice day!\n");
		    
		    //log action
		    AdminLog("Backup Signal Sent to Daemon");
		}
		else 
		{
			printf(ERRMSG);
			AdminErrLog("Invalid Input");
			exit(EXIT_FAILURE);
		}
    } 
    else if(argc == 3)
    {
    	if(strcmp(argv[1], "startup") == 0)
		{
			char *filePath = argv[2];
			AdminLog("Starting up Daemon");
			printf("Argument: %s\n", filePath);
			execl(filePath, filePath, NULL);
			AdminErrLog("Failed to startup");
			printf("\nFailed to startup!\n");
			exit(EXIT_FAILURE);
		}
		else 
		{
			printf(ERRMSG);
			AdminErrLog("Invalid Argument for Startup");
			exit(EXIT_FAILURE);
		}
    }
	else 
	{
		printf(ERRMSG);
		AdminErrLog("Invalid Argument");
		exit(EXIT_FAILURE);
	}
	return 0;
}

void AdminLog(char *message)
{
	//log action
    openlog("Admin", LOG_PID|LOG_CONS, LOG_AUTH);
	syslog(LOG_NOTICE, message);
	closelog();
}
void AdminErrLog(char *message)
{
	//log error
	openlog("Admin", LOG_PID|LOG_CONS, LOG_AUTH);
	syslog(LOG_WARNING ,message);
	closelog();
}
