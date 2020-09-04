#ifndef ETC_DEFINE
#define ETC_DEFINE

#include <signal.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* makePathFromEnvAndName( const char* envName,const char* fileName );
int finde(char *s1,char *s2);
char* xmalloc(size_t len,char *message);
void xfree(void *ptr);
void fehler(char *message);
int frage(char *frage);
void w_clear(WINDOW *w);
void xinit_color();
void highcolor(WINDOW *w,int vc,int hc);
void blinkcolor(WINDOW *w,int vc,int hc);
char* xgetenv(char *name);
void xrun(char *str);
void xdelete(char *str,int pos,int anz);
void xinsert(char *str1,int pos,char *str2);
void sig_abbr(int sig);
void sig_chk(int sig);
void sig_init();
void sig_ignore();

#endif
