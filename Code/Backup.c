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
char* lockperms[] = {"chmod", "-R", "000","/var/www/html/internal", NULL};
char* unlockperms[] = {"chmod", "-R", "777","/var/www/html/internal", NULL};

char* Replace(const char*, char*, char*);

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
	int status = 0;
	char files[1024];
	fp = popen("find /var/www/html/internal -daystart -mtime 0 -print", "r");
	
	while(fgets(files, sizeof(files), fp) != NULL)
	{
		Log("FilePathName", "Just Inside loop");
		//Generate a list of updated files and then
		//overwrite files in live with modified files in internal folders
		FILE* fp2;
		//first we copy the file over to the record, 
		Log("FilePathName", "filename is being created");
		char filename[41] = "/var/www/html/backups/updates/";
		Log("FilePathName", "date is being created");
		char * date = GetDate();
		Log("FilePathName", date);
		strcat(filename, date);
		Log("FilePathName", filename);
		fp2 = fopen(filename, "a");
		fprintf(fp2, "%s", files);
		fclose(fp2);
		
				
		//now get dest folder
		char* destFile = Replace(files, "internal", "live");
		
		//create path
		char pathToFile[1024];
		memset(pathToFile, 0, sizeof(pathToFile));
		strcat(pathToFile, "cp -Ru ");
		strtok(files, "\n");
		strcat(files, " ");
		strcat(files, destFile);
		strcat(pathToFile, files);
		
		fp2 = popen(pathToFile, "r");
		if(fp2 == NULL)
		{
			LogErr("Transfer", "Pipe is broken");
		}
		pclose(fp2);
		
		
		//for day zero internal root dir removal 
		if(strcmp(files,"/var/www/html/internal /var/www/html/live\n") == 0)
		{
			FILE *fp3;
			fp3 = popen("rm -rf /var/www/html/live/internal", "r");
			pclose(fp3);
			LogWarning("Duplicate", "Removed Duplicated root internal folder");
		}
		
	}

	
	status = pclose(fp);
	memset(queueWBuffer, 0, 1024);
    strcpy(queueWBuffer, "Transferred");
    mq_send(mq, queueWBuffer, 1024, 0);
    mq_close(mq);
}

char *Replace(const char *str, char *orig, char *rep)
{
	char buffer[4096];
	char *p;
	
	if(!(p = strstr(str, orig)))	//is orig even in str?
	{
		return str;
	}
	//size_t newlen = strlen(str) - strlen(orig) + strlen(rep);
	//char buffer[newlen+1];
	
	//copy the first segment of the path
	//memcpy(buffer, str, p - str);
	
	//copy the replacement word
	//memcpy(buffer + (p - str), rep, strlen(rep));
	
	//copy the end segment of the path
	//strcpy(buffer +(p - str) + strlen(rep), p + strlen(orig));
	
	//return buffer;
	
	strncpy(buffer, str, p-str);	//copy chars from str to orig
	buffer[p-str] = '\0';
	
	sprintf(buffer + (p-str), "%s%s", rep, p+strlen(orig));

	return buffer;
}
