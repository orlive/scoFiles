extern "C" {

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

#define		L_TASTE	1
#define		R_TASTE	2
#define		B_TASTE 3
#define		LINKS	4
#define		RECHTS	5
#define		RAUF	6
#define		RUNTER	7

#define		REDRAW	for(t=0;t<23;t++)mvwaddstr(w,t+1,1,zl[t])

int main()
{
	WINDOW	*w;
	int	t;
	char	cnt = '0';
	int	x,y;
	char	zl[23][79];

	for( t=0 ; t<23 ; t++ )
		strcpy(zl[t],"                                                                              ");

	strcpy(zl[10],"     MOUSE TEST ");

	x = y = 1;

	w = initscr();

	noecho();
	nodelay(w,TRUE);
	keypad(w,TRUE);

	box(w,0,0);

	REDRAW;

	do
	{
		if( y>23 ) y = 23;
		if( y< 1 ) y = 1;
		if( x>78 ) x = 78;
		if( x< 1 ) x = 1;
			
		wattron(w,A_REVERSE);
		mvwaddch(w,y,x,zl[y-1][x-1]);

		while((t=wgetch(w))==ERR)

		wattroff(w,A_REVERSE);
		mvwaddch(w,y,x,zl[y-1][x-1]);

		switch(t)
		{
			case L_TASTE:
				zl[y-1][x-1] = cnt++;
				REDRAW;
				break;
			case R_TASTE:
				zl[y-1][x-1] = cnt--;
				REDRAW;
				break;
			case RECHTS:	x++;break;
			case LINKS:	x--;break;
			case RAUF:	y--;break;
			case RUNTER:	y++;break;
		}
	}
	while( t!=B_TASTE );

	(void)endwin();
}

}
