#ifndef OS_DIR_DEFINE
#define OS_DIR_DEFINE

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <curses.h>
#include <signal.h>
#include <ctype.h>
#include <assert.h>
#include <sys/times.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <unistd.h>

#include "os.h"

void sort_setup( int nr );
extern int sort_dateien( t_ele *e1 , t_ele *e2 );
void hole_dateien( t_dir *d,char *pfad );
void hole_dateien2( t_dir *d );
void move_dateien( int nr );
void drucke_dateien( int nr );
void pos( int flg , int nr , int y , int x , int i );
void pos2( int nr );
void hole_filter( int nr );
void hole_sort( int nr );
int hole_directory(int nr,char *tmp);
void os_hilfe( void );

#endif
