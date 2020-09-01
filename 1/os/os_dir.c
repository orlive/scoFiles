extern "C" {

#include "os_dir.h"
#include "os.h"
#include "os_be.h"
#include "log.h"
#include "etc.h"
#include "edit.h"
#include "match.h"
#include "eingabe.h"

char tmp[512];
char hlinie[SPALTEN];
int  sort_ordnung;

#define  AKT    d[nr].akt
#define  IDX    d[nr].idx
#define  ANZ    d[nr].anz
#define BALKEN    COLOR_CYAN

void sort_setup( int nr ) {
  sort_ordnung = nr;
}

int sort_dateien( t_ele *e1 , t_ele *e2 ) {
  int  i = 0;
  char *s1,*s2;
    
  switch ( sort_ordnung ) {
    case 0: /* dateiname */
      i = strcmp(e1->name,e2->name);
      break;
    case 1: /* Gr??e (kleinste zuerst) */
      i = (int)(e1->size - e2->size);
      break;
    case 2: /* Gr??e (gr??te   zuerst) */
      i = (int)(e2->size - e1->size);
      break;
    case 3: /* Datum (j?ngste zuerst) */
      if (e1->dir=='d' && e2->dir=='d')
        i = 0;
      else
        i = (int)(e2->mod_time - e1->mod_time);
      break;
    case 4: /* Datum (?lteste zuerst) */
      if (e1->dir=='d' && e2->dir=='d')
        i = 0;
      else
        i = (int)(e1->mod_time - e2->mod_time);
      break;
    case 5: /* Extention */
      s1 = strchr(e1->name,'.');
      s2 = strchr(e2->name,'.');
      if (!s1 && !s2 )
        i = 0;
      else if (!s1)
        i = 1;
      else if (!s2)
        i = -1;
      else
        i = strcmp(s1,s2);
      break;
  }

  if (i==0)
    i = strcmp(e1->name,e2->name);

  if (e1->dir == e2->dir)  /* beide 'd' oder nicht */
    return i;
  if (e2->dir=='d')
    return 1;
  else
    return -1;
}

void hole_dateien(t_dir *d,char *pfad) {
  DIR  *dirp;
  struct  dirent *dp;
  int  i;
  struct  stat  stat_buf;
  struct  passwd  *pw_buf;
  struct  group  *gr_buf;
  struct  tm  *tm_buf;
  char  rechte[11];

  chdir(pfad);
  getcwd(d->akt_dir,510);

  for ( i=0 ; i<d->anz ; i++ )
    xfree( d->e[i].name );

  dirp = opendir( pfad );

  i = 0;

  while ( (dp = readdir( dirp )) != NULL )
  {
    (void)stat (dp->d_name,&stat_buf);  /* liefert int */

    if ( S_ISDIR((ushort)stat_buf.st_mode) )
      d->e[i].dir = 'd';
    else
    {
      d->e[i].dir = ' ';
      if ( strlen(d->filter)>0 )
        if (!match(dp->d_name,d->filter))
          continue;
    }

    if ( strlen(d->filter)>0 )  /* Filter gesetzt ? */
      d->zeilen = getmaxy(stdscr)-6-1;
    else
      d->zeilen = getmaxy(stdscr)-6;
    d->e[i].mark = ' ';
    if (d->e[i].dir!='d')
      d->e[i].size = stat_buf.st_size;
    else
      d->e[i].size = 0;
    d->e[i].name = xmalloc(strlen(dp->d_name)+1,"Verzeichnis - Eintrag");
    strcpy(d->e[i].name,dp->d_name);

    strcpy(rechte,"----------");
    if (d->e[i].dir=='d')
      rechte[0]='d';
    else if (S_ISBLK(stat_buf.st_mode))
      rechte[0]='b';
    else if (S_ISCHR(stat_buf.st_mode))
      rechte[0]='c';
    else if (S_ISFIFO(stat_buf.st_mode))
      rechte[0]='f';
    //else if (S_ISNAM(stat_buf.st_mode))
      //rechte[0]='p';
    else if (S_ISLNK(stat_buf.st_mode))
      rechte[0]='l';
    else if (S_ISFIFO(stat_buf.st_mode))
      rechte[0]='s';

    if ((stat_buf.st_mode & S_IRUSR)==S_IRUSR ) rechte[1]='r';
    if ((stat_buf.st_mode & S_IWUSR)==S_IWUSR ) rechte[2]='w';
    if ((stat_buf.st_mode & S_IXUSR)==S_IXUSR ) rechte[3]='x';

    if ((stat_buf.st_mode & S_IRGRP)==S_IRGRP ) rechte[4]='r';
    if ((stat_buf.st_mode & S_IWGRP)==S_IWGRP ) rechte[5]='w';
    if ((stat_buf.st_mode & S_IXGRP)==S_IXGRP ) rechte[6]='x';

    if ((stat_buf.st_mode & S_IROTH)==S_IROTH ) rechte[7]='r';
    if ((stat_buf.st_mode & S_IWOTH)==S_IWOTH ) rechte[8]='w';
    if ((stat_buf.st_mode & S_IXOTH)==S_IXOTH ) rechte[9]='x';
    strcpy(d->e[i].rechte,rechte);

    pw_buf = getpwuid(stat_buf.st_uid);
    if (strlen(pw_buf->pw_name)>20)
      pw_buf->pw_name[20]='\0';
    strcpy(d->e[i].usr,pw_buf->pw_name);

    gr_buf = getgrgid(stat_buf.st_gid);
    if (strlen(gr_buf->gr_name)>20)
      gr_buf->gr_name[20]='\0';
    strcpy(d->e[i].grp,gr_buf->gr_name);
    
    /* zeiten holen */

    d->e[i].mod_time = stat_buf.st_mtime;
    tm_buf = (struct tm*) localtime(&stat_buf.st_mtime);
    strftime( d->e[i].zeit,20,"%b %d %H:%M",tm_buf);

    if (++i>=DATEIEN)
      break;
  }

  if ( d->anz==-1 ) {
    d->akt = 0;
    d->idx = 0;
    d->anz = i;
    d->manz = 0;
    d->mbyte = 0;
  }

  closedir( dirp );

  sort_setup( d->ordnung );
  qsort(&d->e[0],d->anz,sizeof(d->e[0]),(int(*)(const void*,const void*))sort_dateien);
}

void hole_dateien2( t_dir *d ) { /* erh?hlt aenderungnen */
  int  u;
  t_dir  d2;
  char  name[100];
  t_ele  *f;
  int  idx,akt;

  strcpy( name , d->e[ d->akt+d->idx ].name );
  idx = d->idx;
  akt = d->akt;

  strcpy( d2.filter , d->filter );
  d2.anz    = -1;
  d2.ordnung  = 0;

  hole_dateien( &d2 , d->akt_dir );

  d2.ordnung  = d->ordnung;
  d2.anzeige  = d->anzeige;

  for ( u=0 ; u<d2.anz ; u++ ) {
    f = (t_ele*) bsearch(  &d2.e[u],
          d->e,
          d->anz,
          sizeof(t_ele),
          (int(*)(const void*,const void*))sort_dateien);

    if ( f ) {
      if ( f->mark=='*' ) {
        d2.e[u].mark = '*';
        d2.mbyte += f->size;
        d2.manz++;
      }
    }
  }

  for ( u=0 ; u<d->anz ; u++ )
    xfree( d->e[u].name );

  memcpy( d , &d2 , sizeof(d2) );

  sort_setup( d->ordnung );
  qsort(&d->e[0],d->anz,sizeof(d->e[0]),(int(*)(const void*,const void*))sort_dateien);

  if ( (u = os_finde_name( d , name )) )
    d->akt = u - idx;
  else
    d->akt = akt;

  d->idx = idx;
}

int os_finde_name( t_dir *d , char *name ) {
  for ( int i=0 ; i<d->anz ; i++ )
    if ( !strcmp( d->e[i].name , name ) )
      return i;

  return 0;
}

void move_dateien(int nr) {
  int  i;
  int  y = 0;
  int  t = 0;
  char  flg = 1;
  char  f_neu;
  char  mark;

  keypad(win[nr],TRUE);

  drucke_dateien(nr);

  while (flg) {
    f_neu = 0;

    if ( flg_int )
      os_int_stop();

    switch (t) {
      case 7:      /* ^G */
      case 268:
        if ( hole_directory(nr,tmp) ) {
          d[nr].anz=-1;  /* akt,idx loeschen */
          hole_dateien( &d[nr] , tmp );
          f_neu++;
        }
        os_refresh_all();
        break;
      case 2:      /* ^B */
        edit("/usr/oli/src/c/os/rm1");
        os_refresh_all();
        break;
      case 1:      /* ^A */
      case 270:
        f_neu++;
        d[nr].mbyte = 0;
        if (d[nr].e[IDX+AKT].mark=='*') {
          for (i=0;i<d[nr].anz;i++)
            d[nr].e[i].mark = ' ';
          d[nr].manz = 0;
        } else {
          for (i=0;i<d[nr].anz;i++) {
            d[nr].e[i].mark = '*';
            if (d[nr].e[i].dir!='d')
              d[nr].mbyte +=
                d[nr].e[i].size;
          }
          d[nr].manz = d[nr].anz;
        }
        break;
      case 6:      /* ^F */
      case 266:
        hole_filter(nr);
        os_init_dir2(nr);
        os_refresh_all();
        break;
      case 5:      /* ^E */
      case 269:
        be_edit();
        os_refresh_all();
        break;
      case 15:    /* ^O */
      case 267:
        hole_sort(nr);
        os_init_dir2(nr);
        os_refresh_all();
        break;
      case 18:    /* ^R */
        os_refresh_all();
        break;
      case 27:
        os_int_stop();
        break;
      case KEY_LEFT:
        if (nr!=1)
          break;
        ungetch('\t');
        pos2(nr);
        return;
      case KEY_RIGHT:
        if (nr!=0)
          break;
        ungetch('\t');
        pos2(nr);
        return;
      case '\t':
        ungetch('\t');
        pos2(nr);
        return;
      case ' ':
        if (d[nr].e[IDX+AKT].mark=='*') {
          d[nr].e[IDX+AKT].mark=' ';
          d[nr].manz--;
          if (d[nr].e[IDX+AKT].dir!='d')
            d[nr].mbyte-=
              d[nr].e[IDX+AKT].size;
        } else {
          d[nr].e[IDX+AKT].mark='*';
          d[nr].manz++;
          if (d[nr].e[IDX+AKT].dir!='d')
            d[nr].mbyte+=
              d[nr].e[IDX+AKT].size;
        }
        break;
      case '\n':
        if (d[nr].e[IDX+AKT].dir!='d')
          break;
        d[nr].anz=-1;  /* akt,idx loeschen */
        strcpy(tmp,d[nr].akt_dir);
        strcat(tmp,"/");
        strcat(tmp,d[nr].e[IDX+AKT].name);
        hole_dateien( &d[nr] , tmp );
        f_neu++;
        break;
      case '-':
        f_neu++;
        d[nr].anzeige=
          (d[nr].anzeige==0)?2:d[nr].anzeige-1;
        break;
      case '+':
        f_neu++;
        d[nr].anzeige=
          (d[nr].anzeige==2)?0:d[nr].anzeige+1;
        break;
      case 265:  /* F1 */
        os_hilfe();
        os_refresh_all();
        break;
      case '!':
        xrun("sh");
        os_wininit();
        os_refresh_all();
        break;
      default:
        if (t>='a' && t<='z') {
          int  t2 = toupper(t);
          int  u = AKT + IDX + 1;

          for ( i=0 ; i<ANZ ; i++ ) {
            if (d[nr].e[u].name[0]==t || d[nr].e[u].name[0]==t2) {
              AKT=u-IDX;
              f_neu++;
              break;
            }
            if (++u>=ANZ)
              u=0;
          }
        } else if (t>='1' && t<='8') {
          f_neu++;
          be_wahl(nr,func,t-'1');
        }
    }

    pos(0,nr,1+AKT,1,IDX+AKT );

    switch (t) {
      case KEY_DOWN:
        AKT++;
        break;
      case KEY_UP:
        AKT--;
        break;
      case KEY_NPAGE:
        IDX+=d[nr].zeilen;
        f_neu++;
        break;
      case KEY_PPAGE:
        IDX-=d[nr].zeilen;
        f_neu++;
        break;
      case 262:    /* POS1 */
        AKT=0;
        break;
      case 360:    /* Ende */
        AKT=d[nr].zeilen-1;
        break;
    }

    if (AKT>=d[nr].zeilen-1) {
      IDX += AKT-d[nr].zeilen+1;
      AKT = d[nr].zeilen-1;
      f_neu++;
    } else if (AKT<0) {
      IDX += AKT;
      AKT = 0;
      f_neu++;
    }

    if (IDX>=ANZ) {
      IDX=ANZ-1;
      AKT=d[nr].zeilen-1;
      f_neu++;
    } else if (IDX<0) {
      IDX=0;
      AKT=0;
      f_neu++;
    }
    if (IDX+AKT>=ANZ)
      AKT=ANZ-IDX-1;

    if (f_neu)
      drucke_dateien(nr);

    show_status(nr);
    wrefresh(win[nr]);

    flg_int = 0;

    t = wgetch(win[nr]);

    if (0)
      mvwprintw(func,1,1,"%d",t);wrefresh(func);
  }
}

void show_status(int nr) {
  highcolor(win[nr],COLOR_BLUE,COLOR_BLACK);
  wmove(win[nr],getmaxy(win[nr])-2+1,1);
  wprintw(win[nr],"[");
  highcolor(win[nr],COLOR_WHITE,COLOR_BLACK);
  wprintw(win[nr],"%3d von %3d, (*): %3d (%10ld)",
    d[nr].idx+d[nr].akt+1,
    d[nr].anz,
    d[nr].manz,
    d[nr].mbyte);
  highcolor(win[nr],COLOR_BLUE,COLOR_BLACK);
  wprintw(win[nr],"]");

  pos(1,nr,1+d[nr].akt,1,d[nr].idx+d[nr].akt);
}

void drucke_dateien(int nr) {
  int  i;
  int  u;

  for (i=0;i<d[nr].zeilen;i++) {
    u = d[nr].idx+i;

    if ( u < d[nr].anz )
      pos(0,nr,1+i,1,u);
    else
      mvwprintw(win[nr],1+i,1,"%38s"," ");
  }

  highcolor(win[nr],COLOR_BLUE,COLOR_BLACK);
  box(win[nr],0,0);

  if (d[nr].idx>0)
    mvwaddch(win[nr],1,getmaxx(win[nr])-1,ACS_UARROW);
  if (d[nr].idx+d[nr].zeilen<d[nr].anz)
    mvwaddch(win[nr],getmaxy(win[nr])-2,getmaxx(win[nr])-1,ACS_DARROW);

  u=strlen(d[nr].filter);
  if (u>0) {
    u=SPALTEN-u-4;
    wmove(win[nr],getmaxy(win[nr])-6,0);
    waddch(win[nr],ACS_LTEE);
    wprintw(win[nr],"[%s]",d[nr].filter);
    mvwprintw(win[nr],getmaxy(win[nr])-6,1,"[%s]",d[nr].filter);
    for (i=0;i<u;i++)
      waddch(win[nr],ACS_HLINE);
    waddch(win[nr],ACS_RTEE);
  }

  mvwprintw(win[nr],0,1,"[%s]",d[nr].akt_dir);
  highcolor(win[nr],COLOR_WHITE,COLOR_BLACK);
  mvwprintw(win[nr],0,2,"%s",d[nr].akt_dir);
  if (strlen(d[nr].filter)>0)
    mvwprintw(win[nr],getmaxy(win[nr])-6,2,d[nr].filter);
}

void pos(int flg,int nr,int y,int x,int i) {
  char tmp[512];
  char name[100];

  if (d[nr].ordnung==5)  /* extention */
    sprintf(name,"%15s",d[nr].e[i].name);
  else
    sprintf(name,"%-15s",d[nr].e[i].name);

  switch (d[nr].anzeige) {
    case 0:
      sprintf(tmp," %c%s %s %8ld %10s",
          d[nr].e[i].mark,
          name,
          d[nr].e[i].rechte,
          (long)d[nr].e[i].size,
          " ");
      break;
    case 1:
      sprintf(tmp," %c%s %c %-8s %-8s %-30s",
          d[nr].e[i].mark,
          name,
          d[nr].e[i].dir,
          d[nr].e[i].usr,
          d[nr].e[i].grp,
          " ");
      break;
    case 2:
      sprintf(tmp," %c%s %c %-30s ",
          d[nr].e[i].mark,
          name,
          d[nr].e[i].dir,
          d[nr].e[i].zeit );
      break;
  }
    
  tmp[SPALTEN-2] = '\0';

  if (flg) {
    highcolor(win[nr],COLOR_BLUE,COLOR_WHITE);
    wattron(win[nr],A_REVERSE);
    mvwprintw(win[nr],y,x,"%s",tmp);
    wattroff(win[nr],A_REVERSE);
  } else {
    if ( d[nr].e[i].dir == 'd' )
      highcolor(win[nr],COLOR_MAGENTA,COLOR_BLACK);
    else
      highcolor(win[nr],BALKEN,COLOR_BLACK);
    mvwprintw(win[nr],y,x,"%s",tmp);
  }

  wmove(win[nr],y,x+1);
}

void pos2(int nr) {
  pos(0,nr,1+AKT,1,IDX+AKT );

  highcolor(win[nr],COLOR_BLUE,COLOR_WHITE);
  wattron(win[nr],A_REVERSE);
  mvwaddch(win[nr],AKT+1,1        ,'>');
  mvwaddch(win[nr],AKT+1,SPALTEN-2,'<');
  wattroff(win[nr],A_REVERSE);

  wrefresh(win[nr]);
}

void hole_filter(int nr) {
  WINDOW  *w;
  int  rc;
  char  tmp[40];

  if (nr==0)
    w = newwin(5,40,10,1);
  else
    w = newwin(5,40,10,38);

  strcpy( tmp,d[nr].filter );
  do {
    highcolor(w,COLOR_WHITE,COLOR_BLUE);

    w_clear(w);
    box(w,0,0);

    mvwprintw(w,0,2,"[ Dateienauswahl ]");
    wrefresh(w);

    highcolor(w,COLOR_WHITE,COLOR_BLACK);
    rc = -1;
    if ( !input(w,2,2,36,tmp))
      break;

    wmove(w,3,2);
    blinkcolor(w,COLOR_WHITE,COLOR_BLUE);
    switch ((rc=match("oli",d[nr].filter)))
    {
      case -1: wprintw(w,"Klammer in Klammer nicht erlaubt !");   break;
      case -2: wprintw(w,"Zeichen n Klammer in erwartet !");      break;
      case -3: wprintw(w,"Zeichen - nicht aufgeloest !");         break;
      case -4: wprintw(w,"Zeichen - in [] noch nicht erwartet!"); break;
      case -5: 
      case -6: wprintw(w,"Fehler in Klammer");                    break;
      case -7: wprintw(w,"Zu viele Zeichen in Klammer!");         break;
      case -8: wprintw(w,"Klammer nicht geschlossen!");           break;
    }
    if (rc<0) {
      wprintw(w,"  <TASTE>");
      beep();
      (void)wgetch(w);
    }
      
  } while (rc<0);

  if ( rc>=0 )
    strcpy( d[nr].filter,tmp );

  delwin(w);
}

void hole_sort(int nr) {
  WINDOW  *w;
  int  rc = 1;
  int  y,t;
  char  mp[6][30] = { "Dateiname",
                      "Gr??e (kleinste zuerst)",
                      "Gr??e (gr??te   zuerst)",
                      "Datum (j?ngste zuerst)",
                      "Datum (?lteste zuerst)",
                      "Extention" };

  y = d[nr].ordnung;

  if (nr==0)
    w = newwin(10,30,6,5);
  else
    w = newwin(10,30,6,45);

  highcolor(w,COLOR_CYAN,COLOR_BLUE);

  w_clear(w);
  box(w,0,0);
  keypad(w,TRUE);

  mvwprintw(w,0,2,"[ Sortierung ]");

  for ( t=0 ; t<6 ; t++ )
    mvwprintw(w,t+2,2," %s ",mp[t]);

  highcolor(w,COLOR_WHITE,COLOR_BLUE);

  while (rc) {
    mvwaddch(w,y+2, 2,'[');
    mvwaddch(w,y+2,26,']');

    wrefresh(w);

    flg_int = 0;
    t = wgetch(w);
    if ( flg_int )
      t = 27;

    mvwaddch(w,y+2, 2,' ');
    mvwaddch(w,y+2,26,' ');

    switch (t) {
      case KEY_UP:   y = (y==0) ? 5 : y-1; break;
      case KEY_DOWN: y = (y==5) ? 0 : y+1; break;
      case '\n':  d[nr].ordnung = y;
      case 27: rc = 0;
        break;
    }
  }

  delwin(w);
}

int hole_directory(int nr,char *tmp) {
  WINDOW  *w;
  int  rc;
  int  loop = 1;
  char  direc[60] = "";

  if (nr==0)
    w = newwin(5,60,10,1);
  else
    w = newwin(5,60,10,18);

  do {
    highcolor(w,COLOR_WHITE,COLOR_BLUE);

    w_clear(w);
    box(w,0,0);

    mvwprintw(w,0,2,"[ Wechsle zu Verzeichnis ]");
    wrefresh(w);

    highcolor(w,COLOR_WHITE,COLOR_BLACK);

    rc   = 0;
    loop = 0;

    if ( input(w,2,2,56,direc) ) {
      if (direc[0]=='/')
        strcpy(tmp,direc);
      else
        sprintf(tmp,"%s/%s",d[nr].akt_dir,direc);
      if ( chdir(tmp)==0 )  /* verzeichnis existiert */
        rc = 1;
      else {
        loop= 1;
        blinkcolor(w,COLOR_WHITE,COLOR_BLUE);
        wmove(w,3,2);
        wprintw(w,"Verzeichnis existiert nicht!");
        wprintw(w,"  <TASTE>");
        beep();
        (void)wgetch(w);
      }
    }

  } while (loop);

  delwin(w);

  return(rc);
}

void os_hilfe() {
  WINDOW *w;
  int    rc;
  int    loop = 1;
  char   direc[60] = "";

  w = newwin(12,36,5,21);

  highcolor(w,COLOR_WHITE,COLOR_BLUE);

  w_clear(w);
  box(w,0,0);

  mvwprintw(w,0,2,"[ Hifle zu OS ]");

  mvwprintw(w,2,2,"F1  = Hilfe");
  mvwprintw(w,3,2,"F2  = Dateienauswahl");
  mvwprintw(w,4,2,"F3  = Sortierung");
  mvwprintw(w,5,2,"F4  = Wechsle zu Verzeichnis");
  mvwprintw(w,6,2,"F5  = Befehl editieren");
  mvwprintw(w,7,2,"F6  = Alle Dateien markieren");

  mvwprintw(w,9,2,"ESC = OS verlassen");

  wrefresh(w);

  (void)wgetch(w);

  delwin(w);
}

}
