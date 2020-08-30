#include <match2.h>

#define ANZ_ppos 200
#define debugmatch 1

struct {
	int     f_gueltig;     /* wenn gesetzt, kann hier jeder beliebige
				  buchstabe stehen */
	char    gueltig[256];  /* gÅltige werte */
	int     f_fixppos;      /* fixepposition, wenn nicht gesetzt,bezieht
				  muss buchstabe an dieser stelle ge-
				  funden werden,ansonsten unmatch!
			       */
	}ppos[ANZ_ppos];
	int     anz;           /* anzahl ppositionen */
char   str[ANZ_ppos+1];         /* string in dem inhalt gesucht wird */

int     recu;
int     durchl;

match(char *str1,char *str2)
{
	int     i;

	kill_space(str1);
	kill_space(str2);

	anz    = 0;
	recu   = 0;
	durchl = 0;
	strcpy(str,str1);

	for( i=0 ; i<strlen(str2) ; i++ )     /* analisieren von str2 und erstellen von ppos-tabelle */
	{
		switch( str2[i] )
		{
			case '*':
				ppos[anz].f_fixppos = 1;
				break;
			case '[':
				i = get_klammer(str2,i + 1);   /* anlysiere [a-z} */
				break;
			case '?':
				ppos[anz].f_gueltig = 1;
				anz++;
				break;
			default:
				strcpy(ppos[anz].gueltig," ");
				ppos[anz].gueltig[0] = str2[i];
				anz++;
				break;
		}
	}

	return( vergleich(0,0) );
}

get_klammer(char *str2,int p)
{
	int     i,u;
	int     l;
	char    z;
	char    tmp[1000];

	for( i=p ; i<strlen(str2) ; i++ )     /* analisieren von str2 und erstellen von ppos-tabelle */
	{
		l = strlen(ppos[anz].gueltig);
		if( l )
			z = ppos[anz].gueltig[l-1];
		else
			z = 0;

		switch(str2[i])
		{
			case '[':
				fehler("Klammer in Klammer nicht erlaubt !");
			case ']':
				if(!l)
					fehler("Zeichen in [] erwartet !");
				else if( z == '-' )
					fehler("Zeichen - nicht aufgelîst !");
				anz++;
				return( i );
			case '-':
				if(!l)
					fehler("Zeichen - in [] ncoh nicht erwartet !");
			default:
				switch(z)
				{
					case '-':
						if(str2[i-2]>str2[i])
						{
							sprintf(tmp,"Fehler in Klammer: %c%c%c !",str2[i-2],z,str2[i]);
							fehler(tmp);
						}
						strcpy(tmp," ");
						ppos[anz].gueltig[l-1] = '\0';
						for(u=str2[i-2]+1;u<=str2[i];u++)
						{
							if(strlen(ppos[anz].gueltig)>=255)
								fehler("Zu viele Zeichen in Klammer!");
							tmp[0] = u;
							strcat(ppos[anz].gueltig,tmp);
						}
						break;
					default:
						if(strlen(ppos[anz].gueltig)>=255)
							fehler("Zu viele Zeichen in Klammer!");
						strcpy(tmp," ");
						tmp[0] = str2[i];
						strcat( ppos[anz].gueltig,tmp );
						break;
				}
		}
	}
	fehler("Klammer nicht geshlossen!");
}

vergleich(int p1,int p2)
{
	int     i;
	int     w=0;
	int     li=-1;           /* letzter fixppos punkt i-wert*/
	int     lw=-1;           /* letzter fixppos punkt w-wert*/
	char    tmp[] = " ";

	for(i=p2;i<anz;i++)
	{
		if(p1+w>=strlen(str))
			return(0);

		if( ppos[i].f_fixppos )
		{
			li = i;
			lw = w;
		}

		if( !ppos[i].f_gueltig )                              /* gilt ja immer , da <?>   */
			if( !strchr(ppos[i].gueltig,str[p1+w]) )     /* kein gueltiger buchstabe */
				if( !ppos[i].f_fixppos )
				{
					if( li > -1 )
					{
						w = lw+1;
						i = li-1;
						continue;
					}
					return(0);
				}
				else
				{
					recu++;
					if(vergleich(p1+w+1,i))
					{
						recu--;
						return(1);
					}
					recu--;
					i--;
					return(0);
				}
		w++;

		if(i>=anz)
		{
			if(p1+w<strlen(str) && !ppos[anz].f_fixppos)
				return(0);
		}
	}

	return(1);
}

kill_space(char *str)
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

