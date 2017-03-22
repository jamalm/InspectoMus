#include "log.h"
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

//for standard logs from user activity
void Log(char* name, char* message)
{
    openlog(name, LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_INFO, message);
    closelog();
}

//for warnings
void LogWarning(char* name, char* message)
{
    openlog(name, LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_WARNING, message);
    closelog();
}

//for errors
void LogErr(char* name, char* message)
{
    openlog(name, LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_ERR, message);
    closelog();
}

//for the daemon's personal logs
void LogDaemon(char* name, char* message)
{
    openlog(name, LOG_PID|LOG_CONS, LOG_DAEMON);
    syslog(LOG_ALERT, message);
    closelog();
}
