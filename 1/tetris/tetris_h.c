#include <stdio.h>
#include <sys/types.h>
#include <curses.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "tetris.h"

extern void input(WINDOW* w,int y,int x,int l,char* str);
extern int finde(char *s1,char *s2);

// Deklarations 
void highscore_save(t_highscore *tab);
int highscore_load(t_highscore *tab);
void highscore_creat(t_highscore *tab);
void highscore_draw(WINDOW *hs,t_highscore *tab);
void get_time(char *ptr);

void w_clear(WINDOW *w)
{
	int	x,y;
	int	i;
	char	tmp[81] = "                                                                                ";

	getmaxyx(w,y,x);
		
	tmp[x-2] = '\0';

	y--;
	for (i=1;i<y;i++)
		mvwprintw(w,i,1,"%s",tmp);
}
void highscore(t_score *score)
{
	WINDOW		*hs;
	t_highscore	tab[10];
	int		i;

	if ( !highscore_load(&tab[0]) )
		return;

	hs = newwin(16,66,6,7);		/* Info Fenster */

	if (COLOR_PAIRS>-1)
		wattron(hs,COLOR_PAIR(5));
	else
		wattron(hs,A_REVERSE);

	box(hs,0,0);
	w_clear(hs);

	mvwprintw(hs,0,2,"%s","< Tetris - Highscore >");

	mvwprintw(hs,2, 6,"%s","Name");
	mvwprintw(hs,2,27,"%s","Datum");
	mvwprintw(hs,2,37,"%s","Zeit");
	mvwprintw(hs,2,43,"%s","Linien  Level Punkte");

	highscore_draw(hs,&tab[0]);

	if (!score)
		while (wgetch(hs)==ERR);
	else
		for (i=0;i<10;i++)
			if (score->points > tab[i].points) {
				char	datum[16];
					
				if (i<9)
					memmove(&tab[i+1].name,
						&tab[i].name,
						sizeof(t_highscore)*(9-i));
				strcpy(tab[i].name,"");
				get_time(datum);
				strcpy(tab[i].datum,datum);
				tab[i].lines = score->lines;
				tab[i].level = score->level;
				tab[i].points = score->points;
				highscore_draw(hs,&tab[0]);
				(void)echo();
				nodelay(hs,FALSE);
				input(hs,i+4,6,20,(char*)&tab[i].name);
				/*mvwscanw(hs,i+4,6,"%20s\n",&tab[i].name);*/
				nodelay(hs,TRUE);
				(void)noecho();
				highscore_draw(hs,&tab[0]);
				highscore_save(&tab[0]);
				break;
			}
	delwin(hs);
}

void highscore_draw(WINDOW *hs,t_highscore *tab)
{
	int	i;

	for (i=0;i<10;i++)
		mvwprintw(hs,i+4,2,"%2d) %-20s %-15s %6d %6d %6d",
					i+1,
					tab[i].name,
					tab[i].datum,
					tab[i].lines,
					tab[i].level,
					tab[i].points);
	wrefresh(hs);
}

int highscore_load(t_highscore *tab)
{
	char	*ptr;
	FILE	*datei;
	int	i;

	ptr = (char*)getenv("TETRIS");

	if (!ptr)
		return 0;

	datei = fopen(ptr,"r");
	if (!datei)
		highscore_creat(&tab[0]);
	else
	{
		if ( fread(&tab[0],1,sizeof(t_highscore)*10,datei)
				!= sizeof(t_highscore)*10 )
			highscore_creat(&tab[0]);

		fclose(datei);
	}
	return 1;
}

void highscore_save(t_highscore *tab)
{
	char	*ptr;
	FILE	*datei;
	int	i;

	ptr = (char*)getenv("TETRIS");

	if (!ptr)
		return;

	datei = fopen(ptr,"w");

	fwrite(&tab[0],1,sizeof(t_highscore)*10,datei);

	fclose(datei);
}

void highscore_creat(t_highscore *tab)
{
	int	i;

	for (i=0;i<10;i++) {
		sprintf(tab[i].name,"Mister %2d",i);
		sprintf(tab[i].datum,"21.05.95, 00:49");
		tab[i].lines = 0;
		tab[i].level = 0;
		tab[i].points = 10000-1000*i;
	}
}

void get_time(char *ptr)
{
	time_t	zeit;
	char	*zeitptr;
	char	monate[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

	(void) time(&zeit);
	zeitptr = (char*)ctime(&zeit);


	ptr[0] = zeitptr[8];		/* TAG */
	ptr[1] = zeitptr[9];
	ptr[2] = '.';

	zeitptr[7] = '\0';		/* MONAT */
	sprintf(&ptr[3],"%2d.",(finde(monate,&zeitptr[4])+3)/3);
	if (ptr[3]==' ')
		ptr[3]='0';

	ptr[6] = zeitptr[22];		/* JAHR */
	ptr[7] = zeitptr[23];
	ptr[8] = ',';
	ptr[9] = ' ';
	ptr[10] = '\0';

	zeitptr[16] = '\0';		/* Uhrzeit */
	strcat(ptr,&zeitptr[11]);
}
