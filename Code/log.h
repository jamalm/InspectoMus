#ifndef LOG_H
#define LOG_H

void LogDaemon(char* name,char* message);
void LogErrDaemon(char *name, char *message);
void LogWarning(char* name, char* message);
void LogErr(char* name, char* message);
void Log(char* name, char* message);

#endif
//LOG_H
