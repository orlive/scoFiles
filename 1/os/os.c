#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <ctype.h>
#include <assert.h>
#include <sys/times.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

#include "os.h"
#include "os_dir.h"

#define D { FILE* out=fopen("log.log","at"); fprintf(out,"%s/%d\n",__FILE__,__LINE__); fflush(out); fclose(out); }
extern char* xgetenv(char *name);

WINDOW	*winh,
	*win[2],
	*func;

t_dir	d[2];

char	flg_get_old;

main(int argc,char **argv)
{
	int	i;
	int	nr = 0;
	int	t;
	extern int	optind;
	extern char	*optarg;

	flg_get_old = 1;

	while ( (t = getopt(argc,argv, "mn")) != -1)
	{
		switch(t)
		{
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

	for(;;)
	{
		move_dateien(nr);
		if( (t=wget_taste(win[nr])) )
			switch(t)
			{
			case '\t':
				nr = 1-nr;
				chdir(d[nr].akt_dir);
				break;
			}
	}
		
	(void)wget_taste(win[0]);

	os_uninit();
}

void os_usage(void)
{
	fprintf(stderr,"\nAufruf: os [-m][-n]\n");
	fprintf(stderr,"\t-m: Monodarstellung\n");
	fprintf(stderr,"\t-n: nutze nicht gespeicherte Directories\n\n");
	exit(1);						
}

void os_init_dir( void )
{
	int	i;

	for(i=0;i<2;i++)
	{
		d[i].anz = -1;
		hole_dateien( &d[i] , d[i].akt_dir );
		drucke_dateien(i);
		pos2(i);
		wrefresh(win[i]);
	}
}

void os_init_dir2( int nr )
{
	hole_dateien2( &d[nr] );
	drucke_dateien(nr);
	pos2(nr);
	wrefresh(win[nr]);
}

void os_init( void )
{
	short	i,u;

	sig_ignore();

	winh	= initscr();
	win[0]	= newwin(ZEILEN+2,40,0,0);
	win[1]	= newwin(ZEILEN+2,40,0,40);
	func	= newwin(4,80,20,0);

	start_color();
	xinit_color();	/* etc.c */

	os_wininit();

	os_get_old();
	
}

void os_get_old( void )
{
	FILE	*datei = (FILE*)0;
	int	i,l;
	char	tmp[21];
	char	eingabe[100] = "";

	/* hole alte verzeichnisse */
	strcpy(eingabe,(char*)xgetenv("OS"));
	strcat(eingabe,"/os.dirs");


	if( flg_get_old )
		datei = fopen(eingabe,"r");

	if(datei)
	{
		for( i=0 ; i<2 ; i++ )
		{
			fgets( d[i].akt_dir , 512 , datei );
			if( (l=strlen(d[i].akt_dir)) )
				d[i].akt_dir[l-1] = 0;
			fgets( d[i].filter , 51 , datei );
			if( (l=strlen(d[i].filter)) )
				d[i].filter[l-1] = 0;
			fgets(tmp,20,datei);d[i].anzeige = atoi(tmp);
			fgets(tmp,20,datei);d[i].ordnung = atoi(tmp);
		}
		fclose(datei);
	}
	else
		for( i=0 ; i<2 ; i++ )
		{
			getcwd(d[i].akt_dir,510);
			strcpy(d[i].filter,"");
			d[i].anzeige=0;
			d[i].ordnung=0;
		}

	os_init_dir();
}

void os_set_old( void )
{
	FILE	*datei;
	int	i,l;
	char	tmp[21];
	char	eingabe[100] = "";

	strcpy(eingabe,(char*)xgetenv("OS"));
	strcat(eingabe,"/os.dirs");

	datei = fopen(eingabe,"w");

	if(datei)
	{
		for( i=0 ; i<2 ; i++ )
		{
log_print( d[i].akt_dir );
			fprintf( datei , "%s\n" , d[i].akt_dir );
			fprintf( datei , "%s\n" , d[i].filter );
			fprintf( datei , "%d\n" , d[i].anzeige );
			fprintf( datei , "%d\n" , d[i].ordnung );
		}
		fclose(datei);
	}
}

void os_wininit( void )
{
	(void)noecho();

	/*
	nodelay(winh	,TRUE);
	nodelay(win[0]	,TRUE);
	nodelay(win[1]	,TRUE);
	nodelay(func	,TRUE);
	*/

	keypad(winh	,TRUE);
	keypad(win[0]	,TRUE);
	keypad(win[1]	,TRUE);
	keypad(func	,TRUE);
}

void os_uninit( void )
{
	/*
	nodelay(winh,FALSE);
	nodelay(win[0],FALSE);
	nodelay(win[1],FALSE);
	nodelay(func,FALSE);
	*/
	(void)refresh();
	(void)echo();
	(void)clear();
	(void)endwin();

	os_set_old();

	exit(1);
}

void os_refresh_all( void )
{
	touchwin(winh);
	touchwin(func);
	touchwin(win[0]);
	touchwin(win[1]);

	if(0)
	{
		wrefresh(winh);
		wrefresh(func);
		wrefresh(win[0]);
		wrefresh(win[1]);
	}
	else
	{
		wnoutrefresh(winh);
		wnoutrefresh(func);
		wnoutrefresh(win[0]);
		wnoutrefresh(win[1]);
		doupdate();
	}
}

void os_int_stop()
{
	if(frage("Wirklich beenden ? (j/n)"))
		os_uninit();
	os_refresh_all();
}
