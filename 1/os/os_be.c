extern "C" {

#include "os_be.h"
#include "etc.h"
#include "eingabe.h"
#include "log.h"

char  c_n[8][8] = {
      "Schwarz",
      "Rot",
      "Grün",
      "Gelb",
      "Blau",
      "Orange",
      "Türkis",
      "Weiß" };

t_button  button[BEFEHLE];

void hole_button() {
  FILE  *datei;
  int  i = 0;
  char  tmp[21];
  int   l;
  char *eingabe = (char*)malloc( strlen(xgetenv("OS"))+20 );
  
  strcpy(eingabe,xgetenv("OS"));
  strcat(eingabe,"/os.init");

  if (!(datei = fopen(eingabe,"rt")) )
    fehler("Kann Datei os.init nicht einlesen !");

  free(eingabe);

  for (i=0;i<BEFEHLE;i++)
  {
    fgets(button[i].name,10,datei);
    if (!button[i].name[0])
      break;
    l = strlen(button[i].name) - 1;
    if (l>=0)
      button[i].name[l] = '\0';

    fgets(tmp,20,datei);  button[i].vc = atoi(tmp);
    fgets(tmp,20,datei);  button[i].hc = atoi(tmp);
  }
  fclose(datei);
}

void speicher_button() {
  FILE  *datei;
  int  i = 0;
  char *eingabe = (char*)malloc( strlen(xgetenv("OS"))+20 );
  
  strcpy(eingabe,xgetenv("OS"));
  strcat(eingabe,"/os.init");

  if (!(datei = fopen(eingabe,"wt")) )
    fehler("Kann Datei os.init nicht oeffnen !");

  free(eingabe);

  for (i=0;i<BEFEHLE;i++)
  {
    fprintf(datei,"%s\n",button[i].name);
    fprintf(datei,"%d\n",button[i].vc);
    fprintf(datei,"%d\n",button[i].hc);
  }
  fclose(datei);
}

void drucke_button_all(WINDOW *w) {
  int  x;
  int  c;

  for (x=0;x<=7;x++)
    drucke_button(0,w,x);
  wrefresh(w);
}

void drucke_button(int flg,WINDOW *w,int x) {
  int  p,y;

  p = x;
  x *= 10;

  for (y=0;y<4;y++)
  {
    if (p>=BEFEHLE)
      break;

    if (strlen(button[p].name)>0)
      if (flg)
      {
        blinkcolor(w,button[p].vc,button[p].hc);
        mvwprintw(w,y,x,"<%-8s>",button[p].name);
      }
      else
      {
        highcolor(w,button[p].vc,button[p].hc);
        mvwprintw(w,y,x,"[%-8s]",button[p].name);
      }
    else
    {
      highcolor(w,COLOR_RED,COLOR_BLACK);
      mvwprintw(w,y,x,"          ");
    }
    p+=8;
  }
}

void be_wahl(int nr,WINDOW *w,int x) {
  int  t;

  drucke_button(1,w,x);    /* Spalte blinkend darstellen */
  highcolor(w,7,0);
  mvwprintw(w,1,1,"%s","");  /* weiß auf schwarz erzwingen */
  wrefresh(w);

  flg_int = 0;
  t = wgetch(w);
  if ( flg_int )
    t = 27;

  if (t>='1' && t<='4')    /* befehls-wahl */
  {
    be_schreibe_dateien(nr);
    chdir(d[nr].akt_dir);
    be_befehl(w,x+1,t-'1'+1);  /* 1/1 bis 8/4 */

    os_init_dir2(0);
    os_init_dir2(1);
    os_refresh_all();
  }

  drucke_button(0,w,x);    /* Spalte normal darstellen */
  wrefresh(w);
}

void be_schreibe_dateien(int nr) {
  char  *os;
  char  name[100] = "";
  FILE  *datei;
  int  i;
  
  os = xgetenv("OS");

  /* aktuelle datei ----------------------------------------------*/

  strcpy(name,os);
  strcat(name,"/os.akt");

  if (!(datei = fopen(name,"wt")) )
    fehler("Kann Datei os.akt nicht oeffnen !");

  i = d[nr].akt + d[nr].idx;
  fprintf(datei,"%s",d[nr].e[i].name);

  fclose(datei);

  /* markierte dateien -------------------------------------------*/

  strcpy(name,os);
  strcat(name,"/os.mark");

  if (!(datei = fopen(name,"wt")) )
    fehler("Kann Datei os.mark nicht oeffnen !");

  for (i=0;i<d[nr].anz;i++)
    if ( d[nr].e[i].mark == '*' )
      fprintf(datei,"%s\n",d[nr].e[i].name);

  fclose(datei);

  /* alle dateien (keine directories)-----------------------------*/

  strcpy(name,os);
  strcat(name,"/os.all");

  if (!(datei = fopen(name,"wt")) )
    fehler("Kann Datei os.all nicht oeffnen !");

  for (i=0;i<d[nr].anz;i++)
    if (d[nr].e[i].dir != 'd')  /* kein directory */
      fprintf(datei,"%s\n",d[nr].e[i].name);

  fclose(datei);

  /* aktuelles directories ---------------------------------------*/

  strcpy(name,os);
  strcat(name,"/os_dir.akt");

  if (!(datei = fopen(name,"wt")) )
    fehler("Kann Datei os_dir.akt nicht oeffnen !");

  fprintf(datei,"%s\n",d[nr].akt_dir);

  fclose(datei);

  /* anderes directories -----------------------------------------*/

  strcpy(name,os);
  strcat(name,"/os_dir.other");

  if (!(datei = fopen(name,"wt")) )
    fehler("Kann Datei os_dir.other nicht oeffnen !");

  fprintf(datei,"%s\n",d[1-nr].akt_dir);

  fclose(datei);

}

void be_befehl(WINDOW *w,int t1,int t2) {
  char  *os;
  char  str[1024] = "";

  os = xgetenv("OS");

  sprintf(str,"sh %s/os.%d%d",os,t1,t2);

  xrun(str);
  os_wininit();
}

void be_edit() {
  WINDOW *w;
  int  t;
  char  be_nr[3] = "";
  int  nr;
  int  akt = 0;
  t_button  but;

  w = newwin(7,42,3,19);

  highcolor(w,COLOR_WHITE,COLOR_BLUE);
  w_clear(w);
  box(w,0,0);

  mvwprintw(w,0,2,"%s","[ Befehl editieren ]");
  for (;;)
  {
    highcolor(w,COLOR_WHITE,COLOR_BLUE);

    be_nr[0] = '\0';

    mvwprintw(w,2,2,"Befehlsnr.: [  ]");
    if (!input(w,2,15,2,be_nr))
      break;

    if (be_nr[0]<'1' || be_nr[0]>'8' || be_nr[1]<'1' || be_nr[1]>'4')
    {
      printf("%c",7);
      continue;
    }

    nr  = be_nr[0]-'1' + (be_nr[1]-'1')*8;
    akt  = 0;

    strcpy( but.name,button[nr].name );
    but.hc = button[nr].hc;
    but.vc = button[nr].vc;

    for (;;)
    {
      highcolor(w,COLOR_WHITE,COLOR_BLUE);
      mvwprintw(w,4,2,
        "           Farben  Skript  Speichern ");
      highcolor(w,but.vc,but.hc);
      mvwprintw(w,4,3,"%-8s",but.name);
      wrefresh(w);

      highcolor(w,COLOR_WHITE,COLOR_BLUE);

      switch (akt)
      {
      case 0:
        mvwprintw(w,4,2,"[");
        mvwprintw(w,4,11,"]");
        break;
      case 1:
        mvwprintw(w,4,12,"[Farben]");
        break;
      case 2:
        mvwprintw(w,4,20,"[Skript]");
        break;
      case 3:
        mvwprintw(w,4,28,"[Speichern]");
        break;
      }

      flg_int = 0;
      t = wgetch(w);
      if ( flg_int ) 
        t = 27;

      switch (t)
      {
      case '\n':
        if (akt==0)
        {
          highcolor(w,but.vc,but.hc);
          if (!input(w,4,3,8,but.name))
            strcpy(but.name,
              button[nr].name);
        }
        else if (akt==1)
        {
          be_edit_farben(&but.vc,&but.hc);
          os_refresh_all();
          touchwin(w);
          wrefresh(w);
        }
        else if (akt==2)
        {
          char  *os;
          char  str[1024];

          os = xgetenv("OS");
          sprintf(str,"vi %s/os.%s",os,be_nr);

          xrun(str);
        }
        else if (akt==3)
        {
          strcpy( button[nr].name,but.name );
          button[nr].hc = but.hc;
          button[nr].vc = but.vc;
          speicher_button();
          drucke_button_all(func);
          t=27;  /* verlassen */
        }
        break;
      case KEY_LEFT:
        akt = (akt==0) ? 3 : akt-1;
        break;
      case KEY_RIGHT:
        akt = (akt==3) ? 0 : akt+1;
        break;
      }
      if (t==27)
        break;
    }
    highcolor(w,COLOR_WHITE,COLOR_BLUE);
    mvwprintw(w,4,1,"%-40s"," ");
  }

  delwin(w);
  drucke_button_all(func);
}

void be_edit_farben(int *vc,int *hc) {
  WINDOW  *w = newwin(12,38,10,21);
  int  i,t;
  int  rc = 1;
  int  v,h;

  v = *vc;
  h = *hc;

  highcolor(w,COLOR_WHITE,COLOR_BLACK);
  w_clear(w);
  box(w,0,0);
  mvwprintw(w,0,1,"[ Farben ]");

  keypad(w,TRUE);

  for (i=0;i<8;i++)
    for (t=0;t<8;t++)
    {
      highcolor(w,t,i);
      wmove(w,t+2,i*4+3);
      wprintw(w," # ");
    }

  while (rc)
  {
    wmove(w,v+2,h*4+3);
    if ( h == COLOR_WHITE )
      highcolor(w,COLOR_BLACK,h);
    else
      highcolor(w,COLOR_WHITE,h);
    wprintw(w,"[");
    highcolor(w,v,h);wprintw(w,"#");
    if ( h == COLOR_WHITE )
      highcolor(w,COLOR_BLACK,h);
    else
      highcolor(w,COLOR_WHITE,h);
    wprintw(w,"]");
    wrefresh(w);

    flg_int = 0;
    t = wgetch(w);
    if ( flg_int )
      t = 27;

    highcolor(w,v,h);
    mvwprintw(w,v+2,h*4+3," # ");

    switch (t)
    {
    case KEY_LEFT:  h = (h==0) ? 0 : h-1; break;
    case KEY_RIGHT:  h = (h==7) ? 7 : h+1; break;
    case KEY_UP:  v = (v==0) ? 0 : v-1; break;
    case KEY_DOWN:  v = (v==7) ? 7 : v+1; break;
    case '\n':  *vc = v;*hc = h;
    case 27:  rc = 0;  break;
    default:
      mvwprintw(w,0,0,"%d",t);
    }
  }

  delwin(w);
}

}
