#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
//#include "defs.h"

//#include <termio.h>
#include <signal.h>

ende(int code);
char keystroke();
void syserr(char *msg);
void fatal(char *msg);

static	struct	termio	tbufsave;

FILE	*tty;
int	fd;

main()
{
	char	taste;

	if( signal(SIGINT,SIG_IGN) != SIG_IGN )
		signal( SIGINT,ende );

	if((fd=open("/dev/tty",O_RDONLY))==-1)
		syserr("tty-open");
	while(1)
	{
		taste = keystroke();
		printf("%c\t%3d\n",taste,taste);
	}
	ende(1);
}

ende(int code)
{
	restore();
	exit(1);
}

char keystroke()
{
	static char buf[10];
	static int total, next;
	static BOOLEAN first = TRUE;
	struct termio tbuf;

	if( first )
	{
		first = FALSE;
		if( ioctl(fd,TCGETA,&tbuf)==-1)
			syserr("ioctl");
		
		tbufsave = tbuf;
		tbuf.c_iflag &= ~(	INLCR
					| ICRNL
					| IUCLC
					| ISTRIP
					| IXON
					| BRKINT );
		tbuf.c_oflag &= ~OPOST;
		tbuf.c_iflag &= ~(ICANON | ISIG | ECHO );
		tbuf.c_cc[4] = sizeof(buf);	/* MIN */
		tbuf.c_cc[5] = 2;		/* TIME */
		if(ioctl(fd,TCSETAF,&tbuf)==-1)
		syserr("ioctl2");
	}
	if(next>=total)
		switch(total=read(0,buf,sizeof(buf)))
		{
			case -1:
				syserr("read");
			case 0:
				fatal("Mysterious EOF");
			default:
				next = 0;
		}
	return(buf[next++]);
}

restore()
{
	if(ioctl(fd,TCSETAF,&tbufsave)==-1)
		syserr("ioctl3");
}

/*- ETC - ROUTINEN -----------------------------------------------------------*/

void syserr(char *msg)
{
	extern int errno, sys__nerr;
	extern char *sys_errlist[];

	fprintf(stderr,"ERROR: %s (%d",msg,errno);

	if(errno>0 && errno<sys_nerr)
		fprintf(stderr,", %s)\n",sys_errlist[errno]);
	else
		fprintf(stderr,")\n");
	exit(1);
}

void fatal(char *msg)
{
	fprintf(stderr,"ERROR: %s\n",msg);
	exit(1);
}
