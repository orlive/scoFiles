#include <stdio.h>
#include <string.h>

#define ANZ_ppos 200

char   str[ANZ_ppos+1];         /* string in dem inhalt gesucht wird      */

int     anz_f;                  /* anzahl fixpos                          */
struct  {
	int     pos;            /* fixpos position                        */
	} fpos[ANZ_ppos];

char    fix_end;                /* fipos am ende                          */

int     anz_p;                  /* anzahl ppositionen                     */
struct {
	int     f_gueltig;      /* wenn gesetzt, kann hier jeder          **
				** beliebige buchstabe stehen             */
	char    gueltig[256];   /* gueltige buchstaben                    */
	} ppos[ANZ_ppos];


int match(char *str1,char *str2);
int get_klammer(char *str2,int p);
int ma_vergleich();
int vergleich(int pos,int start,int ende);
void kill_space(char *str);
int stoprun(int nr,char *x);

#ifdef match_obj

int main(int anz_p,char **para)
{
	if(anz_p==2)
	{
		if( match("tetris.c",para[1]) )
			printf("\nOkay\n");
		else
			printf("\nnicht enthalten\n");
	}
	else if(anz_p==3)
	{
		if( match(para[1],para[2]) )
			printf("\nOkay\n");
		else
			printf("\nnicht enthalten\n");
	}
}

#endif

int match(char *str1,char *str2) {
	int     i;

	kill_space(str1);
	kill_space(str2);

	#ifdef match_obj
		printf("\nSuche >%s< in >%s<\n",str2,str1);
	#endif

	anz_p    = 0;
	anz_f    = 0;
	ppos[0].gueltig[0] = 0;

	strcpy(str,str1);

	for( i=0 ; i<strlen(str2) ; i++ )     /* analisieren von str2 und erstellen von ppos-tabelle */
	{
		switch( str2[i] )
		{
			case '*':
				if(i>0 && str2[i-1]=='*')
					break;
				fpos[anz_f++].pos = anz_p;
				fpos[anz_f].pos = 0;          /* initialisierung des n„chsten */
				break;
			case '[':
				i = get_klammer(str2,i + 1);/*anlysiere [a-z]*/
				if(i<0)
					return i;
				break;
			case '?':
				ppos[anz_p].f_gueltig = 1;
				anz_p++;
				break;
			default:
				strcpy(ppos[anz_p].gueltig," ");
				ppos[anz_p].gueltig[0] = str2[i];
				anz_p++;
				ppos[anz_p].gueltig[0] = 0;
				break;
		}
	}

	fix_end = 0;

	if(anz_f>0)
		if( fpos[anz_f-1].pos>=anz_p )
		{
			fix_end = 1;
			anz_f--;
		}

	#ifdef match_obj
		printf("\n-Einzelpositionen des Suchstrings-------");
		printf("----------------------------------------\n");
		for(i=0;i<anz_p;i++)
			printf("%d) %d <%s>\n",i,ppos[i].f_gueltig,
				ppos[i].gueltig );
		printf("\n-gefundene Sternpositionen--------------");
		printf("----------------------------------------\n");
		for(i=0;i<anz_f;i++)
			printf("%d) %d \n",i,fpos[i].pos );
		if(fix_end)
			printf("Stern am Ende des Suchstrings\n\n");
	#endif

	return( ma_vergleich() );
}

int get_klammer(char *str2,int p)
{
	int     i,u;
	int     l;
	char    z;
	char    tmp[1000];

	for( i=p ; i<strlen(str2) ; i++ )     /* analisieren von str2 und erstellen von ppos-tabelle */
	{
		l = strlen(ppos[anz_p].gueltig);
		if( l )
			z = ppos[anz_p].gueltig[l-1];
		else
			z = 0;

		switch(str2[i])
		{
			case '[':
				return -1;
				/*stoprun(-1,"Klammer in Klammer nicht erlaubt !");*/
			case ']':
				if(!l)
					return -2;
					/*stoprun(-2,"Zeichen [] in erwartet !");*/
				else if( z == '-' )
					return -3;
					/*stoprun(-3,"Zeichen - nicht aufgeloest !");*/
				anz_p++;
				return( i );
			case '-':
				if(!l)
					return -4;
					/*stoprun(-4,"Zeichen - in [] noch nicht erwartet !");*/
			default:
				switch(z)
				{
					case '-':
						if(str2[i-2]>str2[i])
						{
							sprintf(tmp,"stoprun in Klammer: %c%c%c !",str2[i-2],z,str2[i]);
							return -5;
							/*stoprun(-5,tmp);*/
						}
						strcpy(tmp," ");
						ppos[anz_p].gueltig[l-1] = '\0';
						for(u=str2[i-2]+1;u<=str2[i];u++)
						{
							if(strlen(ppos[anz_p].gueltig)>=255)
								return -6;
								/*stoprun(-6,"Zu viele Zeichen in Klammer!");*/
							tmp[0] = u;
							strcat(ppos[anz_p].gueltig,tmp);
						}
						break;
					default:
						if(strlen(ppos[anz_p].gueltig)>=255)
							return -7;
							/*stoprun(-7,"Zu viele Zeichen in Klammer!");*/
						strcpy(tmp," ");
						tmp[0] = str2[i];
						strcat( ppos[anz_p].gueltig,tmp );
						break;
				}
		}
	}
	return -8;
	/*stoprun(-8,"Klammer nicht geschlossen!");*/
}


/****************************************************************************
**                                                                         **
** kopf-steuer-routine fuer die vergleiche                                 **
**                                                                         **
****************************************************************************/


int ma_vergleich() {
	int	i;
	int	start;
	int     ende;
	int     pos  = 0;       /* position in str */
	int     pos2 = 0;
	char    flg  = 0;

				/******************************************
				** Sonderf„lle:                          **
				**    Suchstring = '*'                   **
				**    Suchstring = ''                    **
				******************************************/
	if( anz_p==0 )
		return( fix_end );

				/******************************************
				** Es wurden sind keine sterne im such-  **
				** string vorhanen.                      **
				******************************************/
	if( anz_f==0 || fpos[0].pos!=0 )
	{
		start   = 0;
		if(anz_f>0)
			ende    = fpos[0].pos-1;
		else
			ende    = anz_p-1;

				/******************************************
				** keine sterne und kein stern am ende   **
				******************************************/
		if(!anz_f && !fix_end)
			pos = strlen(str)-(anz_p-start);

				/******************************************
				** suchstring ist groesser als ausgangs- **
				** string                                **
				******************************************/
		if(pos<0)
			return(0);

		pos2 = vergleich(pos,start,ende);

		#ifdef match_obj
			printf("Pos 1. INSTANZ: %d\n",pos2);
		#endif

				/******************************************
				** nicht gefunden !                      **
				******************************************/
		if( pos2 < 0 )
			return 0;

				/******************************************
				** am ende war kein stern: da sonst      **
				** auch kein stern vorhanden war muesste **
				** suchstring am anfang stehen !         **
				******************************************/
		if( !anz_f )
			if( pos2!=0 )
				return 0;

		pos = pos2 + ( ende - start + 1 );
	}

	if(anz_f > 0 )
		for(i=0;i<anz_f-1;i++)
		{
			start   = fpos[ i ].pos;
			ende    = fpos[i+1].pos-1;

			pos2 = vergleich(pos,start,ende);

			#ifdef match_obj
				printf("Pos 2. INSTANZ: %d\n",pos2);
			#endif

				/******************************************
				** nicht gefunden !                      **
				******************************************/
			if( pos2 < 0 )
				return 0;

			pos = pos2 + ( ende - start + 1 );
		}

				/******************************************
				** Hier wird letzte *xx position gesucht **
				** ueberpruefung auf stern am ende des   **
				** suchstrings                           **
				******************************************/
	if(anz_f>0)
	{
		start   = fpos[anz_f-1].pos;

				/******************************************
				** stern am ende des suchstrings ?       **
				******************************************/
		if( !fix_end )
			pos2 = strlen(str)-(anz_p-start);
		else
			pos2 = pos;

				/******************************************
				** rest vom suchstring ist groesser als  **
				** rest vom ausgangsstring               **
				******************************************/
		if(pos2 < pos)
			return 0;

		ende    = anz_p - 1;

		pos = vergleich(pos2,start,ende);

		#ifdef match_obj
			printf("Pos 3. INSTANZ: %d\n",pos);
		#endif

				/******************************************
				** nicht gefunden !                      **
				******************************************/
		if( pos < 0 )
			return 0;
	}

	return(1);
}


/****************************************************************************
**                                                                         **
** vergleichs routine                                                      **
**                                                                         **
****************************************************************************/

int vergleich(int pos,int start,int ende)
{
	int     i,u;
	int     anz;
	char    rc;
	char    tmp[200];

	anz = ende-start + 1;

	#ifdef match_obj
		printf("\n-Neuer Vergleich------------------------");
		printf("----------------------------------------\n");
		printf("pos:%d   start:%d   ende:%d   anz:%d\n",
			pos,start,ende,anz);
		printf("    1234567890123456789012345678901234567890\n");
	#endif

				/******************************************
				** suche ersten buchstabe                **
				******************************************/
	for(i=pos;i<strlen(str);i++)
	{
		if(i+anz>strlen(str))
			return -1;

		#ifdef match_obj
			strcpy(tmp,str);
			tmp[i]='.';
			printf("%2d) %s : %s %d\n",i,tmp,ppos[start].gueltig,
				ppos[start].f_gueltig);
		#endif

			/* gilt ja immer , da <?>   */
			/* kein gueltiger buchstabe */
			if(	!ppos[start].f_gueltig &&
				!strchr(ppos[start].gueltig,str[i]) )
				continue;
			else
			{
				rc  = i;
				for(u=1;u<anz;u++)
				{
					rc = i;

					/*if( i+u>=strlen(str)-1 )*/
					if( i+u>=strlen(str) )
						break;

					#ifdef match_obj
						printf("     u: %d  :%s %d\n",u,ppos[start+u].gueltig,ppos[start+u].f_gueltig);
					#endif

					if( !ppos[start+u].f_gueltig )
						if( !strchr(ppos[start+u].gueltig,str[i+u]) )
						{
							rc = -1;
							break;
						}
				}
				if(rc>=0)
					return(rc);
			}
	}

	return(-1);
}

void kill_space(char *str)
{
	int     i;
	char    *z;

	for(z=str;*z==' ';z++);
	strcpy(str,z);

	for(i=strlen(str)-1;i>=0;i--)
		if(str[i]==' ')
			str[i] = 0;
		else
			break;
}

int stoprun(int nr,char *x)
{
	/*printf(x);*/
	return nr;
}
