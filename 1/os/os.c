#include "controller.h"
extern "C" {

#define NOEXTERN 
#include "os.h"
#include "os_dir.h"
#include "os_be.h"
#include "etc.h"
#include "log.h"

char flg_get_old = 1;

int main( int argc , char *argv[] ) {
  int nr = 0;
  int t;

  while ( (t = getopt(argc,argv, "tmn")) != -1) {
    switch (t) {
      case 't': { // test
          initscr();
          //printf("test:%c\n", mvwgetch(winh,0,0) );
          refresh();
          int rc = frage("Wirklich beenden ? (j/n)");
          endwin();
          return rc;
        }
        break;
      case 'm':
        COLOR_PAIRS=-1;
        break;
      case 'n':
        flg_get_old = 0;
        break;
      default:
        os_usage();
    }
  }

  os_init();
  hole_button();
  drucke_button_all(func);

  controllerInit();

  for (;;) {
    move_dateien(nr);
    if ( (t=wgetch(win[nr])) ) {
      if ( t=='\t' ) {
        nr = 1-nr;
        chdir(d[nr].akt_dir);
      }
    }
  }
    
  wgetch(win[0]);

  os_uninit();
}

void os_usage() {
  fprintf(stderr,"\nAufruf: os [-m][-n]\n");
  fprintf(stderr,"\t-m: Monodarstellung\n");
  fprintf(stderr,"\t-n: nutze nicht gespeicherte Directories\n\n");
  exit(1);            
}

void os_init_dir() {
  for ( int i=0 ; i<2 ; i++) {
    d[i].anz = -1;
    hole_dateien( &d[i] , d[i].akt_dir );
    drucke_dateien(i);
    pos2(i);
    wrefresh(win[i]);
  }
}

void os_init_dir2( int nr ) {
  hole_dateien2( &d[nr] );
  drucke_dateien(nr);
  pos2(nr);
  wrefresh(win[nr]);
}

void os_init() {
  sig_ignore();

  initscr();

  win[0] = newwin(getmaxy(stdscr)-4,getmaxx(stdscr)/2,0,0);
  win[1] = newwin(getmaxy(stdscr)-4,getmaxx(stdscr)/2,0,getmaxx(stdscr)/2);
  func   = newwin(4,getmaxx(stdscr),getmaxy(stdscr)-4,0);

  start_color();
  xinit_color();

  os_wininit();
  os_get_old();
}

void os_get_old() {
  FILE  *datei = (FILE*)0;
  char  tmp[21];
  char  eingabe[100] = "";
  int   l;

  /* hole alte verzeichnisse */
  strcpy(eingabe,xgetenv("OS"));
  strcat(eingabe,"/os.dirs");

  if ( flg_get_old )
    datei = fopen(eingabe,"r");

  if (datei) {
    for ( int i=0 ; i<2 ; i++ ) {
      fgets( d[i].akt_dir , 512 , datei );
      if ( (l=strlen(d[i].akt_dir)) )
        d[i].akt_dir[l-1] = 0;
      fgets( d[i].filter , 51 , datei );
      if ( (l=strlen(d[i].filter)) )
        d[i].filter[l-1] = 0;
      fgets(tmp,20,datei);d[i].anzeige = atoi(tmp);
      fgets(tmp,20,datei);d[i].ordnung = atoi(tmp);
    }
    fclose(datei);
  } else {
    for ( int i=0 ; i<2 ; i++ ) {
      getcwd(d[i].akt_dir,510);
      strcpy(d[i].filter,"");
      d[i].anzeige=0;
      d[i].ordnung=0;
    }
  }

  os_init_dir();
}

void os_set_old( void ) {
  FILE  *datei;
  char  tmp[21];
  char  eingabe[100] = "";

  strcpy(eingabe,xgetenv("OS"));
  strcat(eingabe,"/os.dirs");

  datei = fopen(eingabe,"w");

  if (datei) {
    for ( int i=0 ; i<2 ; i++ ) {
      fprintf( datei , "%s\n" , d[i].akt_dir );
      fprintf( datei , "%s\n" , d[i].filter );
      fprintf( datei , "%d\n" , d[i].anzeige );
      fprintf( datei , "%d\n" , d[i].ordnung );
    }
    fclose(datei);
  }
}

void os_wininit() {
  noecho();

  keypad(stdscr ,TRUE);
  keypad(win[0] ,TRUE);
  keypad(win[1] ,TRUE);
  keypad(func   ,TRUE);
}

void os_uninit() {
  refresh();
  echo();
  clear();
  endwin();

  os_set_old();
  exit(1);
}

void os_refresh_all() {
  touchwin(func);
  touchwin(win[0]);
  touchwin(win[1]);

  if (0) {
    wrefresh(stdscr);
    wrefresh(func);
    wrefresh(win[0]);
    wrefresh(win[1]);
  } else {
    wnoutrefresh(stdscr);
    wnoutrefresh(func);
    wnoutrefresh(win[0]);
    wnoutrefresh(win[1]);
    doupdate();
  }
}

void os_int_stop() {
  if (frage("Wirklich beenden (j/n) ? "))
    os_uninit();
  os_refresh_all();
}

}
