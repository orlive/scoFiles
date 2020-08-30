#ifndef LOG_DEFINE
#define LOG_DEFINE

#include <stdio.h>
#include <stdlib.h>

int log_open();
void log_close();
void log_print(char* s1);
void debug_print();
void debug_clear();

#endif
