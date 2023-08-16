#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <curses.h>
#include "f.h"

WINDOW *hw;

char	datei[256];

unsigned char	font[4096];
unsigned char	bit[8] = { 128,64,32,16,8,4,2,1 };

main(int argc,char **argv)
{
	int	i,u;

	if( argc==1 )
		exit(1);

	strcpy(datei,argv[1]);

	FontInit();

	FontHaupt();

	FontUninit();
}

void FontInit(void)
{
	hw = initscr();
	(void) noecho();
	(void) keypad(hw,TRUE);
}

void FontUninit(void)
{
	(void) echo();
	(void) endwin();
	exit(1);
}

void FontHaupt(void)
{
	FontLaden();

	FontVidi();

	FontDrawAll();

	FontWahl();

	FontSpeichern();
}

void FontWahl(void)
{
	int	x,y;
	int	t;

	x = y = 0;

	do{

		if( x<0 ) x=15;
		if( x>15) x=0;
		if( y<0 ) y=15;
		if( y>15) y=0;

		FontDrawOne(y*16+x);

		mvwaddch( hw , y + 1 , x*2 + 20 , '(' );
		mvwaddch( hw , y + 1 , x*2 + 22 , ')' );

		t = getch();

		mvwaddch( hw , y + 1 , x*2 + 20 , ' ' );
		mvwaddch( hw , y + 1 , x*2 + 22 , ' ' );

		switch(t)
		{
			case KEY_DOWN:
				y++;
				break;
			case KEY_UP:
				y--;
				break;
			case KEY_LEFT:
				x--;
				break;
			case KEY_RIGHT:
				x++;
				break;
			case '\n':
				FontEdit(y*16+x);
				FontSpeichern();
				FontVidi();
				break;
				break;
		}

	} while( t!=27 );
	
}

void FontEdit(int nr)
{
	int	x,y;
	int	t;
	unsigned char	a;

	x = y = 0;

	do{

		if( x<0 ) x=7;
		if( x>7 ) x=0;
		if( y<0 ) y=15;
		if( y>15) y=0;

		a = font[nr*16+y];
		wmove(hw,y+1,x+1);

		if( (a & bit[x]) == bit[x] )
			waddch( hw,'*' );
		else
			waddch( hw,'.' );

		wmove(hw,y+1,x+1);

		t = getch();

		switch(t)
		{
			case KEY_DOWN:
				y++;
				break;
			case KEY_UP:
				y--;
				break;
			case KEY_LEFT:
				x--;
				break;
			case KEY_RIGHT:
				x++;
				break;
			case ' ':
				a = font[nr*16+y];
				if( (a & bit[x]) == bit[x] )
					a = a & ~bit[x];
				else
					a = a | bit[x];
				font[nr*16+y] = a;
				break;
		}

	} while( t!='\n' );
	
}

void FontLaden(void)
{
	FILE	*d;

	if( !(d=fopen(datei,"r")) )
		FontExit("Kann Datei nicht lesen");

	if( fread(font,1,4096,d)!=4096 )
		FontExit("Fehler beim Lesen");

	fclose(d);
}

void FontSpeichern(void)
{
	FILE	*d;

	if( !(d=fopen(datei,"w")) )
		FontExit("Kann Datei nicht Schreiben");

	if( fwrite(font,1,4096,d)!=4096 )
		FontExit("Fehler beim Schreiben");

	fclose(d);
}

void FontDrawOne(int nr)
{
	int	u,i,t;
	unsigned char	a;

	for( i=0 ; i<16 ; i++ )
	{
		a = font[nr*16+i];
		wmove(hw,i+1,1);
		for( t=0 ; t<8 ; t++ )
			if( (a & bit[t]) == bit[t] )
				waddch( hw,'*' );
			else
				waddch( hw,'.' );
	}
}

void FontDrawAll(void)
{
	int	i,x,y;

	x = 1;
	y = 1;

	for( i=0 ; i<256 ; i++ )
	{
		wmove(hw,y,2*x+19);

		waddch( hw,i );
		 
		if(++x > 16)
		{
			y++;
			x=1;
		}
	}
}

void FontExit(char *str)
{
	fprintf(stderr,"%s\n",str);
	FontUninit();
}

void FontVidi(void)
{
	char	tmp[500];

	sprintf(tmp,"vidi -f %s font8x16",datei);

	(void)system(tmp);
}
