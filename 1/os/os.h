#ifndef OS_DEFINE
#define OS_DEFINE

#include <unistd.h>
#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <ctype.h>
#include <assert.h>
#include <sys/times.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#define ZEILEN  18
#define SPALTEN 40
#define  BEFEHLE 32
#define DATEIEN  500

typedef struct  {
  char   dir;         // 'd'=directory
  char   mark;        // markierungsflag
  off_t  size;        // dateigroesse
  char   rechte[11];  // zugriffsrechte
  char   usr[21];     // username
  char   grp[21];     // groupname
  char   *name;       // dateiname
  time_t mod_time;    // zeit
  char   zeit[21];    // zeit
} t_ele;

typedef  struct  {
  char  akt_dir[512]; // aktuelle Verzeichnis
  char  filter[51];   // filter beim einlesen
  int   ordnung;      // art der sortierung
  int   anzeige;      // art der dateidarstellung
  int   zeilen;       // Anzahl der zeilen
  t_ele e[DATEIEN];   // elemente
  int   manz;         // anzahl markierte dateien
  long  mbyte;        // gesamt groesse markierte
  int   anz;          // anzahl der dateien
  int   akt;          // aktuell angezeigte datei
  int   idx;          // nr der ersten angezeigten datei
} t_dir;

typedef struct {
  char name[9];       // name des befehls
  int  hc,vc;         // vorder-, hintergrundfarbe
} t_button;

#ifdef  NOEXTERN 
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int  flg_int;
EXTERN int  flg_quit;
EXTERN WINDOW *winh,*win[2],*func;
EXTERN t_dir  d[2];

#undef NOEXTERN
#undef EXTERN

void os_usage( void );
void os_init_dir( void );
void os_init_dir2( int nr );
int os_finde_name( t_dir *d , char *name );
void os_init( void );
void os_get_old( void );
void os_set_old( void );
void os_wininit( void );
void os_uninit( void );
void os_refresh_all( void );
void os_int_stop( void );


#endif
