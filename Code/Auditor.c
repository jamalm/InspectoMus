#include "Auditor.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>

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
        char* auditAddRules[] = {"auditctl", "-w","/var/www/html", "-p","wa", "-k","User_Watch",NULL};
    	execv("/sbin/auditctl", auditAddRules);
    	LogErr("Auditd", "Error Adding Rules");
    	exit(127);
}


