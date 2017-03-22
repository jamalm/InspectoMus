#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "log.h"
#include "timer.h"
#include "Backup.h"

//for locking/unlocking directories
char* lockperms[] = {"chmod", "-R", "700","/var/www/html", NULL};
char* unlockperms[] = {"chmod", "-R", "775","/var/www/html", NULL};

char* Replace(char*, char*, char*);

void Lockup(char* Qname)
{
	mqd_t mq;
	char queueWBuffer[1024];
	mq = mq_open(Qname, O_WRONLY);
	
    memset(queueWBuffer, 0, 1024);
    strcpy(queueWBuffer, "Locked");
    mq_send(mq, queueWBuffer, 1024, 0);
    mq_close(mq);

	Log("File Access", "Locking");
	execvp("chmod", lockperms);
	LogErr("File Access", "Failed");
	exit(127);
}

void Unlock(char* Qname)
{
	mqd_t mq;
	char queueWBuffer[1024];
    mq = mq_open(Qname, O_WRONLY);
    
	Log("File Access", "Unlocking");
	
	        

    memset(queueWBuffer, 0, 1024);
    strcpy(queueWBuffer, "Unlocked");
    mq_send(mq, queueWBuffer, 1024, 0);
    mq_close(mq);
            
	execvp("chmod", unlockperms);
	LogErr("File Access", "Failed");
	exit(127);
}

void Backup(char* Qname)
{
    mqd_t mq;
    char queueWBuffer[1024];
    mq = mq_open(Qname, O_WRONLY);

	Log("Backup", "Backing up files now");
	//tar over all files and name tar with date

	char pathToFile[1024]; 
	strcat(pathToFile,"/var/www/html/backups/");
	strcat(pathToFile, GetDate());
	strcat(pathToFile, ".tar.gz");


	char* tarCommands[] = {"tar", "-cpzf", pathToFile, "/var/www/html/live", NULL};
	
	memset(queueWBuffer, 0, 1024);
    strcpy(queueWBuffer, "Backed Up");
    mq_send(mq, queueWBuffer, 1024, 0);
    mq_close(mq);
    
	execvp("tar", tarCommands);
	LogErr("Backup", "Failed");
	exit(127);
}

void Transfer(char* Qname)
{
    mqd_t mq;
    char queueWBuffer[1024];
    mq = mq_open(Qname, O_WRONLY);
    
	Log("Backup", "Transferring files now");
	//only transfer newly modified files
	FILE *fp;
	int status;
	char files[1024];
	fp = popen("find /var/www/html/internal -mtime 0 -print", "r");
	
	while(fgets(files, sizeof(files), fp) != NULL)
	{
		FILE* fp2;
		//overwrite files in live with modified files in internal folders
		//first get dest folder
		char* destFile = Replace(files, "internal", "live");
		//create path
		char pathToFile[1024];
		strcat(pathToFile, "cp ");
		strcat(pathToFile, files);
		strcat(pathToFile, " ");
		strcat(pathToFile, destFile);
		Log("Transferring",files);
		
		fp2 = popen(pathToFile, "w+");
		//pclose(fp2);
	}
	
	status = pclose(fp);
	memset(queueWBuffer, 0, 1024);
    strcpy(queueWBuffer, "Transferred");
    mq_send(mq, queueWBuffer, 1024, 0);
    mq_close(mq);
}

char* Replace(char* directory, char* orig, char* new)
{
	static char buffer[4096];
	char *p;
	
	if(!(p = strstr(directory, orig)))	//if orig is not in directory
		return directory;
		
	strncpy(buffer, directory, p-directory);	//copy chars from directory start to orig start
	buffer[p-directory] = '\0';
	
	sprintf(buffer+(p-directory), "%s%s", new, p+strlen(orig));
	
	return buffer;
}
