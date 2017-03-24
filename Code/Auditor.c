#include "Auditor.h"
#include "log.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RemoveRules()
{
	    Log("Auditd", "Removing Old Rules");
    	char* auditDelRules[] = {"auditctl", "-D", NULL};
		printf("\nRemoving old Rules..");
    	//clear list of rules
    	execv("/sbin/auditctl", auditDelRules);
    	LogErr("Auditd", "Error Removing Rules");
    	exit(127);
}

void AddRules()
{
	    //add rules 
        Log("Auditd", "Adding Rules");
        printf("\nAdding Rules to auditer...");
        char* auditAddRules[] = {"auditctl", "-w","/var/www/html/internal", "-p","wa", "-k","User_Watch",NULL};
    	execv("/sbin/auditctl", auditAddRules);
    	LogErr("Auditd", "Error Adding Rules");
    	exit(127);
}

void AuditReport()
{
	//create report 
	FILE *fp;
	char command[512]; 
	Log("Auditd", "Logging Acivity for the day");
	memset(command, 0, sizeof(command));
	strcpy(command, "ausearch -k User_Watch | aureport -f -i -ts today");
	strcat(command, " > /var/www/html/backups/log/");
	strcat(command, GetDate());
	strcat(command, ".log");
	fp = popen(command, "r");

	
	Log("Auditd", "Logged Acivity for the day");
	pclose(fp);
}


