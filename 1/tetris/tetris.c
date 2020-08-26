#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <ctype.h>
#include <assert.h>
#include <sys/times.h>
#include <sys/types.h>
#include "tetris.h"
#include "tetris_f.h"

// Declarations...
void creat_figur(char flg,t_figur *f);
void draw_figur(char flg,WINDOW *w,t_figur *f,int y,int x);
int rnd(int n);
void uninit();


void haupt(t_score *score);
void highscore(t_score *score);

clock_t times();
extern long lrand48();
extern void srand48();

char	scr[25][51];
char	leer_linie[51];
char	voll_linie[51];

int	dimx;	
int	dimy;

WINDOW	*winh,
	*win1,
	*win2,
	*win3,
	*win4;

int main(int argc,char **argv)
{
	extern int	optind;
	extern char	*optarg;
	int	t=0;
	int	x,y;
	char	mono = 0;
	int	level = 1;
	t_score	score;
	char	h_flg;

	while ( (t = getopt(argc,argv, "l:x:y:m")) != -1)
	{
		switch(t)
		{
			case 'l':
				level = atoi(optarg);
				break;
			case 'm':
				mono = 1;
				break;
			case 'x':
				x = atoi(optarg);
				break;
			case 'y':
				y = atoi(optarg);
				break;
			case '?':
			default:
				usage();
		}
	}

	if(level<1 || level>10)
		level = 1;

	if(y>9 && y<21)
		dimy = y;
	else
		dimy = 20;

	if(x>9 && x<47)
		dimx = x;
	else
		dimx = 20;

	init();

	if(mono)
		COLOR_PAIRS = -1;

	for(;!strchr("nN",t);)
	{
		clear();
		box(winh,0,0);
		getmaxyx(winh,y,x);
		y--;
		mvwprintw(winh,0,2,"< TETRIS >");
		mvwprintw(winh,y,72,"[OB95]");
		box(win1,0,0);
		box(win2,0,0);
		box(win3,0,0);
		mvwprintw(win4,0,0,"Nächster:");
		refresh_all();

		score.level	= score.start_level = level;
		score.lines	= 0;
		score.points	= 0;

		haupt(&score);

		highscore(&score);
		h_flg = 1;

		mvwprintw(winh,y,2,"< Spiel Ende ! Neues Spiel (j/n) ? >");
		wrefresh(winh);

		for(t=ERR;!strchr("jJnN",t);t=wgetch(winh))
			switch(t)
			{
			case ERR:
				break;
			default:
				if(h_flg)
				{
					h_flg = 0;
					refresh_all();
					wmove(winh,y,34);
					break;
				}
				if(strchr("hH",t))
				{
					h_flg = 1;
					highscore(&score);
					wmove(winh,y,34);
				}
			}
	}
		
	uninit();
}

usage()
{
	fprintf(stderr,"\nAufruf: tetris [-l <n>] [-y <n>] [-x <n>] [-m]\n");
	fprintf(stderr,"\t-l: n = Level (1-10)\n");
	fprintf(stderr,"\t-y: n = Zeilenanzahl (10-20)\n");
	fprintf(stderr,"\t-x: n = Spaltenanzahl (10-46)\n");
	fprintf(stderr,"\t-m: Monodarstellung\n\n");
	exit(1);						
}

void haupt(t_score *score)
{
	t_figur figur;
	t_figur old_figur;
	t_figur next_figur;
	int	i,t;
	char	tmp[100];
	struct tms tbuf;
	long	tt;
	long	intervall	= ZEIT;
	long	delay		= ZEIT;

	intervall = delay = ZEIT - (score->level-1)*ZEIT_SCHRITT;
	if(intervall<10)
		intervall=delay=10;

	tt = times(&tbuf) + delay;
	t  = 0;				/* eingabe  initialisieren */

	for(i=0;i<dimy;i++)
		strcpy(scr[i],leer_linie);

	creat_figur(0,&next_figur);

	while(1)
	{
		mvwprintw(win1,1,1,"Level :%6d",score->level);
		mvwprintw(win1,2,1,"Linien:%6d",score->lines);
		mvwprintw(win1,3,1,"Punkte:%6d",score->points);
		wrefresh(win1);

		memcpy(&figur,&next_figur,sizeof(figur));
		memcpy(&old_figur,&figur,sizeof(figur));

		for(i=0;i<4;i++)
			mvwprintw(win4,2+i,2,"        ");

		creat_figur(0,&next_figur);
		draw_figur(1,win4,&next_figur,1,1);
		wrefresh(win4);

		draw_scr();
		draw_figur(1,win3,&figur,figur.y,figur.x);
		wrefresh(win3);

		tt	= times(&tbuf) + delay;

		while(1)
		{
			if(tt<=times(&tbuf))
			{
				tt = times(&tbuf) + delay;

				figur.y++;
				if(!chk_figur(&figur))
				{
					figur.y--;
					if(figur.y==1)	/* neue figur */
						return;
					break;
				}
			}

			if( memcmp(&old_figur,&figur,sizeof(figur)) )
			{
				draw_figur(0,win3,&old_figur,
						old_figur.y,old_figur.x);
				draw_figur(1,win3,&figur,figur.y,figur.x);
				wrefresh(win3);
			}

			memcpy(&old_figur,&figur,sizeof(figur));

			switch(t)
			{
			case DREHEN:
			case KEY_UP:
				figur.pos = figur.pos==3 ? 0 : figur.pos+1;
				if(!chk_figur(&figur))
					figur.pos = old_figur.pos;
				break;
			case RECHTS:
			case KEY_RIGHT:
				figur.x+=2;
				if(!chk_figur(&figur))
					figur.x = old_figur.x;
				break;
			case LINKS:
			case KEY_LEFT:
				figur.x-=2;
				if(!chk_figur(&figur))
					figur.x = old_figur.x;
				break;
			case KEY_DOWN:
			case SPEED:
				do
					t=wgetch(winh);
				while(t!=ERR);
				delay = 0;
				tt = times(&tbuf) + delay;
				break;
			case PAUSE:
				wattron(win2,A_BLINK);
				mvwprintw(win2,2,5,"Pause !");
				wattroff(win2,A_BLINK);
				wrefresh(win2);
				do
					t=wgetch(winh);
				while(t==ERR);
				mvwprintw(win2,2,5,"       ");
				wrefresh(win2);
				break;
			case QUIT:
				return;
			case HIGHSCORE:
				highscore((t_score*)0);
				touchwin(winh);
				touchwin(win1);
				touchwin(win2);
				touchwin(win3);
				touchwin(win4);
				wnoutrefresh(winh);
				wnoutrefresh(win1);
				wnoutrefresh(win2);
				wnoutrefresh(win3);
				wnoutrefresh(win4);
				doupdate();
				break;
			}

			if( t>0 && t!=SPEED )		/* Fall gebremst ! */
				delay = intervall;

			t = wgetch(winh);
			if(t==ERR)
				t = 0;
		}
		binde_figur(&figur);
		figur.y++;

		finde_linie(0,score);

		intervall = delay = ZEIT - (score->level-1)*ZEIT_SCHRITT;
		if(intervall<10)
			intervall=delay=10;
	}
}

void creat_figur(char flg,t_figur *f)
{
		if(flg)
			f->nr = AF;
		else
			f->nr = rnd(AF);
		f->pos	= rnd(4);
		f->farbe	= rnd(5)+1;
		f->y = 1;
		f->x = dimx / 2 + ((dimx/2)%2)-2;
}

finde_linie(int flg,t_score *score)
{
	int	i,u;
	char	ja;

	for(i=dimy-1;i>=0;i--)
	{
		ja = 1;
		for(u=0;u<dimx;u+=2)
			if(scr[i][u]!=VOLL_ZEICHEN[0])
			{
				ja=0;
				break;
			}

		if(	ja
			||
			(flg && strcmp(scr[i],leer_linie))	)
		{
			score->lines++;
			score->level=(int)(score->lines/10)+score->start_level;
			score->points += i * score->level;
			strcpy(scr[i],leer_linie);
			draw_scr();
			wrefresh(win3);
			for(u=i;u>0;u--)
				strcpy(scr[u],scr[u-1]);
			strcpy(scr[0],leer_linie);
			draw_scr();
			wrefresh(win3);
			i++;
		}
	}
}

draw_scr()
{
	int	i,u;

	for(i=0;i<dimy;i++)
	{
		wmove(win3,i+1,1);
		for(u=0;u<dimx;u+=2)
			if(scr[i][u]=='[')
			{
				if(COLOR_PAIRS>-1)
				{
					wattron(win3,COLOR_PAIR(scr[i][u+1]));
					wprintw(win3,"  ");
					wattroff(win3,COLOR_PAIR(scr[i][u+1]));
				}
				else
				{
					wattron(win3,A_REVERSE);
					wprintw(win3,"  ");
					wattroff(win3,A_REVERSE);
				}
			}
			else
				wprintw(win3,LEER_ZEICHEN);
	}
}

void draw_figur(char flg,WINDOW *w,t_figur *f,int y,int x)
{
	int i,u;

	for(i=0;i<4;i++)
		for(u=0;u<8;u+=2)
			if(fig[f->nr][f->pos][i][u]!=' ')
			{
				wmove(w,y+i+1,x+u+1);
				if(flg)
				{
					if(COLOR_PAIRS>-1)
					{
						wattron(w,COLOR_PAIR(f->farbe));
						wprintw(w,"  ");
						wattroff(w,COLOR_PAIR(f->farbe));
					}
					else
					{
						wattron(w,A_REVERSE);
						wprintw(w,"  ");
						wattroff(w,A_REVERSE);
					}
				}
				else
					wprintw(w,LEER_ZEICHEN);
			}

	wattroff(w,A_REVERSE);
	wmove(w,0,0);
}

chk_figur(t_figur *f)
{
	int	i,u;

	for(i=0;i<4;i++)
		for(u=0;u<8;u+=2)
			if(fig[f->nr][f->pos][i][u]!=' ')
				if( f->y+i > dimy-1	|| f->x+u > dimx-1
				||  f->y+i < 0		|| f->x+u < 0 )
					return 0;
				else
					if(scr[f->y+i][f->x+u]!=LEER_ZEICHEN[0])
						return 0;
	return 1;
}

binde_figur(t_figur *f)
{
	int	i,u;

	for(i=0;i<4;i++)
		for(u=0;u<8;u+=2)
			if(fig[f->nr][f->pos][i][u]!=' ')
				if( f->y+i > dimy-1	|| f->x+u > dimx-1
				||  f->y+i < 0		|| f->x+u < 0 )
					exit(1);
				else
				{
					scr[f->y+i][f->x+u]
						= fig[f->nr][f->pos][i][u];
					scr[f->y+i][f->x+u+1]
						= f->farbe;
				}
}

int rnd(int n)
{
	return(((lrand48() & 0x7FFF) % n));
}

init()
{
	int	i;

	if( signal(SIGINT,SIG_IGN) != SIG_IGN )
		signal( SIGINT,uninit );

	leer_linie[0] = '\0';
	voll_linie[0] = '\0';

	for(i=0;i<dimx/2;i++)
	{
		strcat(leer_linie,LEER_ZEICHEN);
		strcat(voll_linie,VOLL_ZEICHEN);
	}

	srand48(time(0L)+getpid());	/* Zufallsgenerator starten */

	winh = initscr();
	win1 = newwin(5,16,INFOY,INFOX);		/* Info Fenster */
	win2 = newwin(5,16,STATY,STATX);		/* Status Fenster */
	win3 = newwin(dimy+2,dimx+2,POSY,POSX);		/* Spielfeld */
	win4 = newwin(8,12,NEXTY,NEXTX);		/* Spielfeld */

	start_color();

	init_pair(1,COLOR_WHITE,COLOR_WHITE);
	init_pair(2,COLOR_WHITE,COLOR_RED);
	init_pair(3,COLOR_WHITE,COLOR_YELLOW);
	init_pair(4,COLOR_WHITE,COLOR_GREEN);
	init_pair(5,COLOR_WHITE,COLOR_BLUE);
	init_pair(6,COLOR_WHITE,COLOR_MAGENTA);

	(void)noecho();

	keypad(winh,TRUE);
	nodelay(winh,TRUE);
}

void uninit()
{
	/* setblock(0,TRUE); */
	nodelay(winh,FALSE);
	clear();
	(void)refresh();
	(void)echo();
	(void)endwin();
	exit(0);
}

refresh_all()
{
	touchwin(winh);
	touchwin(win1);
	touchwin(win2);
	touchwin(win3);
	touchwin(win4);
	wnoutrefresh(winh);
	wnoutrefresh(win1);
	wnoutrefresh(win2);
	wnoutrefresh(win3);
	wnoutrefresh(win4);
	doupdate();
}
