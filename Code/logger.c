#include "log.h"
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

void Log(char* name, char* message)
{
    openlog(name, LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_INFO, message);
    closelog();
}

void LogWarning(char* name, char* message)
{
    openlog(name, LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_WARNING, message);
    closelog();
}

void LogErr(char* name, char* message)
{
    openlog(name, LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_ERR, message);
    closelog();
}

void LogDaemon(char* name, char* message)
{
    openlog(name, LOG_PID|LOG_CONS, LOG_DAEMON);
    syslog(LOG_ALERT, message);
    closelog();
}