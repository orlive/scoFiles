extern "C" {

#include <stdio.h>
#include <curses.h>
#include <string.h>

void edit( char *dateiname );
void draw_txt( WINDOW *w );
void load_txt( WINDOW *w,char *dateiname );

extern void highcolor(WINDOW *w,int vc,int hc);
extern void w_clear(WINDOW *w);
extern void fehler(char *message);

char  txt[10][80];
int  txt_cnt;

void edit( char *dateiname ) {
        FILE  *dat;
        WINDOW  *w;
        int  sx = 20;
        int  sy = 10;
        int  t;

        w = newwin(sy+2,sx+2,2,2);
        nodelay(w,TRUE);
        keypad(w,TRUE);
        highcolor(w,COLOR_WHITE,COLOR_BLUE);
        w_clear(w);
        box(w,0,0);

        load_txt(w,dateiname);
        draw_txt(w);

  while ((t=wgetch(w))==ERR);

  delwin(w);
}

void load_txt( WINDOW *w,char *dateiname ) {
  FILE *dat;
  
  txt_cnt  = 0;

  if (!(dat=fopen(dateiname,"r")))
    fehler("EDIT Kann Datei nicht oeffnen !");

  while ( fgets(txt[txt_cnt],80,dat ))
    txt_cnt++;
}

void draw_txt( WINDOW *w ) {
  for ( int i=0;i<txt_cnt;i++ )
    mvwprintw(w,i+1,1,txt[i]);
}

}
