#ifndef MATCH_DEFINE
#define MATCH_DEFINE

#include <stdio.h>
#include <curses.h>
#include <string.h>

int match(char *str1,char *str2);
int get_klammer(char *str2,int p);
int ma_vergleich();
int vergleich(int pos,int start,int ende);
void kill_space(char *str);
int stoprun(int nr,char *x);

#endif
