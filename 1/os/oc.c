#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <ctype.h>
#include <assert.h>
#include <sys/times.h>
#include <sys/types.h>
#include <dirent.h>
#include <malloc.h>

#include "os.h"

WINDOW	*winh,
	*win[2],
	*func;

t_dir	d[2];

void os_int_stop();

main(int argc,char **argv)
{
	int	i;
	int	nr = 0;
	int	t;
	extern int	optind;
	extern char	*optarg;

	while ( (t = getopt(argc,argv, "m")) != -1)
	{
		switch(t)
		{
			case 'm':
				COLOR_PAIRS=-1;
				break;
			default:
				os_usage();
		}
	}

	os_init();
	hole_button();
	drucke_button_all(func);

	for(i=0;i<2;i++)
	{
		strcpy(d[i].akt_dir,(char*)getcwd((char*)0,510));
		strcpy(d[i].filter,"");
		d[i].anzeige=0;
		d[i].ordnung=0;
	}
	os_os_init_dir();

	for(;;)
	{
		move_dateien(nr);
		if((t=wgetch(win[nr]))!=ERR)
			switch(t)
			{
			case '\t':
				nr = 1-nr;
				chdir(d[nr].akt_dir);
				break;
			}
	}
		
	while(wgetch(win[0])==ERR);

	unos_init();
}

os_usage()
{
	fprintf(stderr,"\nAufruf: os [-m]\n");
	fprintf(stderr,"\t-m: Monodarstellung\n\n");
	exit(1);						
}

os_os_init_dir()
{
	int	i;


	for(i=0;i<2;i++)
	{
		d[i].anz = -1;
		hole_dateien(i,d[i].akt_dir);
		drucke_dateien(i);
		pos2(i);
		wrefresh(win[i]);
	}
}

os_init()
{
	short	i,u;

	sig_ignore();

	winh	= os_initscr();
	win[0]	= newwin(ZEILEN+2,40,0,0);
	win[1]	= newwin(ZEILEN+2,40,0,40);
	func	= newwin(4,80,20,0);

	start_color();
	xos_init_color();	/* etc.c */

	winos_init();
}

winos_init()
{
	(void)noecho();

	nodelay(winh	,TRUE);
	nodelay(win[0]	,TRUE);
	nodelay(win[1]	,TRUE);
	nodelay(func	,TRUE);

	keypad(winh	,TRUE);
	keypad(win[0]	,TRUE);
	keypad(win[1]	,TRUE);
	keypad(func	,TRUE);
}

unos_init()
{
	/* setblock(0,TRUE); */
	nodelay(winh,FALSE);
	nodelay(win[0],FALSE);
	nodelay(win[1],FALSE);
	nodelay(func,FALSE);
	(void)refresh();
	(void)echo();
	(void)clear();
	(void)endwin();
	exit(1);
}

os_refresh_all()
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
		unos_init();
	os_refresh_all();
}
