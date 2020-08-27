#include <signal.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	flg_int;
int	flg_quit;

int wget_taste(WINDOW *w);
int finde(char *s1,char *s2);
void* xmalloc(size_t len,char *message);
void xfree(void *ptr);
void fehler(char *message);
int  frage(char *frage);
void w_clear(WINDOW *w);
void xinit_color();
void highcolor(WINDOW *w,int vc,int hc);
void blinkcolor(WINDOW *w,int vc,int hc);
char* xgetenv(char *name);
void xrun(char *str);
void xdelete(char *str,int pos,int anz);
void xinsert(char *str1,int pos,char *str2);
void sig_abbr(int sig);
void sig_chk(int sig);
void sig_init();
void sig_ignore();

extern void os_uninit();

int wget_taste(WINDOW *w)
{
	int	t;

	t = wgetch(w);

	return t;
}

int finde(char *s1,char *s2) {
	int	len;
	char	*c1;

	len = strlen(s2);

	for(c1=s1;*c1;c1++)
		if(!memcmp(c1,s2,len))
			return((int)(c1-s1));
					
	return -1;
}

void* xmalloc(size_t len,char *message)
{
	char	*ptr;

	if( (ptr = (char*)malloc(len)) == NULL )
	{
		os_uninit();
		fprintf(stderr,"Kann keinen Speicher zuordnen: %s\n",message);
		exit(1);
	}
	return ptr;
}

void xfree(void *ptr) {
	if(ptr)
		free(ptr);
}

void fehler(char *message)
{
	WINDOW *w;

	w = newwin(5,78,1,1);
	highcolor(w,7,4);
	w_clear(w);
	box(w,0,0);
	mvwprintw(w,1,2,"Kritischer Abbruch:");
	blinkcolor(w,7,4);
	mvwprintw(w,3,2,"%s",message);

	(void)wget_taste(w);
	os_uninit();
}

int frage(char *frage)
{
	WINDOW	*w;
	int	t;
	int	len;

	len = strlen(frage);

	w = newwin(3,len+4,10,(79-len)/2);

	if(COLOR_PAIRS>-1)
	{
		init_pair(12,COLOR_BLUE,COLOR_WHITE);
		wattron(w,COLOR_PAIR(12));
	}
	else
		wattron(w,A_REVERSE);

	w_clear(w);
	box(w,0,0);

	mvwprintw(w,1,2,"%s",frage);
	wrefresh(w);

	do
	{
		t=wget_taste(w);
	} while(!strchr("jJnN",t));

	delwin(w);

	if(strchr("jJ",t))
		return 1;
	else
		return 0;
}

void w_clear(WINDOW *w)
{
	int	x,y;
	int	i;
	char	tmp[81] = "                                                                                ";

	getmaxyx(w,y,x);
		
	tmp[x-2] = '\0';

	y--;
	for(i=1;i<y;i++)
		mvwprintw(w,i,1,"%s",tmp);
}

void xinit_color()
{
	int	i,u;

	for(i=0;i<8;i++)
		for(u=0;u<8;u++)
			init_pair((short)(i*8+u+1),(short)u,(short)i);
}

void highcolor(WINDOW *w,int vc,int hc)
{
	if(COLOR_PAIRS>-1) {
		wattroff(w,A_BLINK);
		/*wattron(w,A_BOLD);*/
		wattron(w,COLOR_PAIR(hc*8+vc+1));
	}
}

void blinkcolor(WINDOW *w,int vc,int hc)
{
	if(COLOR_PAIRS>-1)
	{
		/*wattron(w,A_BOLD);*/
		wattron(w,A_BLINK);
		wattron(w,COLOR_PAIR(hc*8+vc+1));
	}
}

char* xgetenv(char *name)
{
	char	*c;
	char	tmp[100];

	if(!(c = getenv(name)))
	{
		sprintf(tmp,"Variable %s nicht definiert !",name);
		fehler(tmp);
	}
	return c;
}

void xrun(char *str)
{
	int	status;

	/* reset_shell_mode(); */
	endwin();

	sig_init();
	system(str);
	sig_ignore();

	/* reset_prog_mode(); */
}

/****
 ****	STRINGFUNCTIONS
 ****/

void xdelete(char *str,int pos,int anz)
{
	int	len = strlen(str);

	if( pos<0 || pos>len )
		return;

	if( anz<1 || anz>(pos+len) )
		return;

	memmove(str+pos,str+pos+anz,len-pos);
}

void xinsert(char *str1,int pos,char *str2)
{
	int	len1 = strlen(str1);
	int	len2 = strlen(str2);
	char	*ptr;
	char	*tmp;

	if( pos<0 || pos>len1 )
		return;

	if( len2<=0 )
		return;

	tmp = (char*)xmalloc(len1+1,"xinsert");

	strcpy(tmp,str1+pos);

	sprintf(str1+pos,"%s%s",str2,tmp);

	xfree(tmp);
}

/* signal funktionen */

void sig_abbr(int sig)
{
	fehler("Programm Abbruch");
}

void sig_chk(int sig)
{
	switch(sig) 
	{
		case SIGINT :	signal(SIGINT,sig_chk);  
				flg_int	= 1 ; break;
		case SIGQUIT:	signal(SIGQUIT,sig_chk); 
				flg_quit = 1 ; break;
		case SIGALRM:	signal(SIGALRM,sig_chk); 
				break;
	}
}

void sig_init()
{
	signal(SIGINT,sig_abbr);
	signal(SIGALRM,sig_chk);
}

void sig_ignore()
{
	signal(SIGINT ,sig_chk);
	signal(SIGQUIT,sig_chk);
	signal(SIGALRM,sig_chk);
}
