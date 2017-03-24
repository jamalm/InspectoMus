#include "Auditor.h"
#include "log.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//function to remove the old watch rules before adding new ones
void RemoveRules()
{
	    Log("Auditd", "Removing Old Rules");
    	char* auditDelRules[] = {"auditctl", "-D", NULL};
    	//clear list of rules
    	execv("/sbin/auditctl", auditDelRules);
    	LogErr("Auditd", "Error Removing Rules");
    	exit(127);
}

//function to add new watch rules to directory
void AddRules()
{
	    //add rules 
        Log("Auditd", "Adding Rules");

        char* auditAddRules[] = {"auditctl", "-w","/var/www/html/internal", "-p","wa", "-k","User_Watch",NULL};
    	execv("/sbin/auditctl", auditAddRules);
    	LogErr("Auditd", "Error Adding Rules");
    	exit(127);
}

//logs report at the start of every dormant phase
void AuditReport()
{
	//create report 
	FILE *fp;
	char command[512]; 

	memset(command, 0, sizeof(command));
	strcpy(command, "ausearch -k User_Watch | aureport -f -i -ts today");
	strcat(command, " > /var/www/html/backups/log/");
	strcat(command, GetDate());
	strcat(command, ".log");
	fp = popen(command, "r");

	
	Log("Auditd", "Logged Acivity for the day");
	pclose(fp);
}


