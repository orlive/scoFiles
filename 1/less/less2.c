#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <termio.h>

#define	LINES	20
#define	COLS	80

FILE	*tty;
FILE	*datei;
FILE	*fp;		/* FILE-POINTER */

int	anz_zeile;
int	akt_zeile;
int	akt_col;

char	buf [ BUFSIZ ];
char	buf2[ BUFSIZ ];

main()
{
	int	col;

	init();

	einlesen();

	if(anz_zeile==0)
		ende("kein ");

	tty	= fopen( "/dev/tty","r" );

	datei	= fopen( "rm1",	"r" );
	fp	= fopen( "rmfp","r" );

	akt_zeile	= 1;
	akt_col		= 0;

	next_zeilen();

	while(1)
	{
		sc_uninit();
		fgets(buf,BUFSIZ,tty);
		sc_init();
	
		col = atoi(buf);

		if(col>0)
		{
			akt_col=col;
			next_zeilen();
			continue;
		}

		switch(buf[0])
		{
			case 'q':
				uninit();
				exit(1);
			case '1':
				akt_zeile=1;
				break;
			case 'g':
			case 'G':
				akt_zeile=anz_zeile-LINES;
				if(akt_zeile<1)
					akt_zeile=1;
				break;
			case '+':
				if(akt_zeile<anz_zeile)
					akt_zeile++;
				break;
			case '-':
				if(akt_zeile>1)
					akt_zeile--;
				break;
			case '0':
				akt_col = 0;
				break;
			default:
				if( akt_zeile+LINES<=anz_zeile )
					akt_zeile+=LINES;
		}
		next_zeilen();
	}

	uninit();
}

einlesen()
{
	int	taste;
	int	i,u;
	int	l1,l2;
	char	t[]	= " ";

	anz_zeile = 0;

	if((datei=fopen("rm1","w"))==0)
		ende("Fehler beim Öffnen von rm1\n");

	if((fp=fopen("rmfp","w"))==0)
		ende("Fehler beim Öffnen von rmfp\n");

	while(fgets(buf,1000,stdin))
	{
		l1 = strlen(buf);
		buf2[0] = '\0';

		for(i=0;i<l1;i++)
			if(buf[i]=='\t')
			{
				l2 = 8 - strlen(buf2) % 8;
				for(u=0;u<l2;u++)
					strcat(buf2," ");
			}
			else
			{
				t[0] = buf[i];
				strcat(buf2,t);
			}

		fprintf( fp,	"%-16ld\n",	ftell(datei) );
		fprintf( datei,	"%s",		buf2);
		anz_zeile++;
	}
	fclose( datei );
	fclose( fp );
	
	uninit();
}

next_zeilen()
{
	int	i;
	int	cnt = 0;
	long	pos;
	char	*b;
	char	linial[COLS+1] = "";

	fseek( fp,(long)(akt_zeile-1)*17,SEEK_SET );

	/*
	printw("\n");
	printw("-----------------------------------------------\n");
	printw("Position in FP			%ld\n",ftell(fp));
	*/

	fgets(buf,1000,fp);
	pos = atol(buf);
	fseek( datei,pos,SEEK_SET );

	/*
	printw("gelesene Position aus rmfp (buf)%ld\n",pos);
	printw("gelesene Position aus rmfp	%ld\n",pos);
	printw("neue Position in Datei		%ld\n",ftell(datei));
	printw("-----------------------------------------------\n");
	*/

	for(i=0;i<COLS/5;i++)
	{
		sprintf(buf,"%-5d\n",i);
	}
	printw(buf);

	while( cnt<=LINES && akt_zeile+cnt++<=anz_zeile )
	{
		fgets(buf,1000,datei);

		if(strlen(buf)>akt_col)
		{
			b = buf+akt_col;
			b[COLS]='\0';
		}
		else
			strcpy(b,"\n");

		printw("%s",b);
	}
	for(i=cnt;i<=LINES;i++)
		printw("~\n");
		
	printw("1.Zeile: %d von %d : ",akt_zeile,anz_zeile);

	refresh();
}

ende(char *s)
{
	uninit();
	printw("\n%s\n",s);
	exit(0);
}

/*----------------------------------------------------------------------------*/

int_ende(int code)
{
	clear();
	printw("Vom Benutzer abgebrochen !");
	refresh();
	sc_uninit();
	exit(code);
}

init()
{
	sc_init();

	if( signal(SIGINT,SIG_IGN) != SIG_IGN )
		signal( SIGINT,int_ende );
}

uninit()
{
	sc_uninit();
}

/*-- BILDSCHIRMSTEUERUNG -----------------------------------------------------*/

sc_init()
{
	initscr();
	/*cbreak();*/
	/*noecho();*/
}

sc_uninit()
{
	endwin();
}

