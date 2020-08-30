#ifndef OS_BE_DEFINE
#define OS_BE_DEFINE

#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <sys/types.h>
#include <unistd.h>
#include "os.h"

void hole_button();
void speicher_button();
void drucke_button_all(WINDOW *w);
void drucke_button(int flg,WINDOW *w,int x);
void be_wahl(int nr,WINDOW *w,int x);
void be_schreibe_dateien(int nr);
void be_befehl(WINDOW *w,int t1,int t2);
void be_edit();
void be_edit_farben(int *vc,int *hc);

#endif
