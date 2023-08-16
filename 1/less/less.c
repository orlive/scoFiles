/* HIER KOMMT EINE TESTZEILE HIN DIE LAENGER ALS 80 ZEICHEN IST UM ZU SEHEN WIE DAS MIT DEM ZEILENUMBRUCH FUNKTIONIERT */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define	LINES	23
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
	int	i;
	int	loop = 1;

	einlesen();

	if(anz_zeile==0)
		ende("");

	tty	= fopen( "/dev/tty","r" );

	datei	= fopen( "rm1",	"r" );
	fp	= fopen( "rmfp","r" );

	akt_zeile	= 1;
	akt_col		= 0;

	while( loop )
	{
		if( akt_zeile > anz_zeile )
			akt_zeile = anz_zeile;
		else if( akt_zeile < 1 )
			akt_zeile = 1;

		next_zeilen();

		fgets(buf,BUFSIZ,tty);

		if( isdigit( buf[0] ) )
		{
			i = atoi(buf);

			if( i>0 )
			{
				akt_col = i;
				continue;
			}
		}

		switch(buf[0])
		{
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
				i = atoi( buf+1 );
				if( i>0 )
					akt_zeile+=i;
				else
					akt_zeile++;
				break;
			case '-':
				i = atoi( buf+1 );
				if( i>0 )
					akt_zeile-=i;
				else
					akt_zeile--;
				break;
			case 'q':
			case 'Q':
				loop = 0;
				break;
			default:
				if( akt_zeile+LINES<=anz_zeile )
					akt_zeile+=LINES;
		}
	}
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
}

next_zeilen()
{
	int	i;
	int	cnt = 0;
	long	pos;
	char	*b;

	fseek( fp,(long)(akt_zeile-1)*17,SEEK_SET );

	/*
	printf("\n");
	printf("-----------------------------------------------\n");
	printf("Position in FP			%ld\n",ftell(fp));
	*/

	fgets(buf,1000,fp);
	pos = atol(buf);
	fseek( datei,pos,SEEK_SET );

	/*
	printf("gelesene Position aus rmfp (buf)%ld\n",pos);
	printf("gelesene Position aus rmfp	%ld\n",pos);
	printf("neue Position in Datei		%ld\n",ftell(datei));
	printf("-----------------------------------------------\n");
	*/

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

		printf("%s",b);
	}
	for(i=cnt;i<=LINES;i++)
		printf("~\n");
		
	printf("SPALTE(%d) ZEILE(%d VON %d) ",akt_col,akt_zeile,anz_zeile);
}

ende(char *s)
{
	printf("\n%s\n",s);
	exit(0);
}
