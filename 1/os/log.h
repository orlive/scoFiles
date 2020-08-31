#ifndef LOG_DEFINE
#define LOG_DEFINE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define D logPrintf(__FILE__,__LINE__,"")

int log_open();
void log_close();
void logPrintf(const char *callerFile,int callerLine,const char *msg, ...);
void log_print(char* s1);
void debug_print();
void debug_clear();

#endif
