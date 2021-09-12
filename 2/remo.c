#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE    *datei_read;
FILE    *datei_write;

char    werte_screen[100][10];
char    werte_attrib[100][32];
int     werte_attrib_nr[100];

char    f_bild=0;
char    f_ein=0;
char    f_attr=0;

int     ws_anz=0;
int     wa_anz=0;
char    ws_nr=0;
char    wa_nr=0;

void hole_felder( char zeile[] );
void hole_attrib( char zeile[] );
void fehler( char meldung[] );
int finde_cmp( char zeile[],char str[] );
int finde( char zeile[],char str[] );
void fehler( char meldung[] );
void strlwr( char *str );

int main(int u1,char *u2[])	{
	char *test;
	char *test2;
	char zeile[100];
	char b;
	int	i;

	if( u1<=1 )
		fehler("Keine Datei gewaehlt");

	datei_read = fopen( u2[1] , "rt" );
	if( !datei_read )
		fehler("Fehler beim Oeffnen von Maske...");

	datei_write = fopen( "remo1" , "wt" );
	if( !datei_write )
		fehler("Fehler beim Oeffnen von \"remo1\"...");

	do	{
		test = fgets( zeile,99,datei_read );

		if( test )
			{
			f_ein=0;
			ws_nr=0;

			if( finde( zeile,"attributes" ) )
				f_attr = 1;

			if( f_attr )
				if( finde( zeile,"end" ) )
					f_attr = 0;

			if( f_attr )
				hole_attrib( zeile );
			else
				hole_felder( zeile );

			}
		} while( test );

	fclose( datei_read );

	printf("\nGefunden Felder in SREEN :\n\n");
	for( i=0 ; i<ws_anz ; i++ )
		printf( "%s\n",werte_screen[i] );

	printf("\nGefunden Felder in ATTRIBUTES :\n\n");
	for( i=0 ; i<wa_anz ; i++ )
		printf( "%s\n",werte_attrib[i] );

	printf("\nFelder in EINGABEREIHENFOLGE :\n\n");
	for( i=0 ; i<ws_anz-1 ; i++ )
		{
		printf( "%s,\n",werte_attrib[ werte_attrib_nr[i] ] );
		fprintf( datei_write,"%s,\n",werte_attrib[ werte_attrib_nr[i]]);
		}
	printf( "%s\n",werte_attrib[ werte_attrib_nr[ws_anz-1] ] );
	fprintf( datei_write,"%s\n",werte_attrib[ werte_attrib_nr[ws_anz-1] ]);

	if( ws_anz>wa_anz )
		printf("\nWARNUNG: Mehr SCREEN-Felder als ATTRIBUTE-Felder!\n");
	if( ws_anz<wa_anz )
		printf("\nWARNUNG: Mehr ATTRIBUTE-Felder als SCREEN-Felder!\n");
	}

void hole_felder( char zeile[] ) {
	int     i;
	char    b;

	for( i=0 ; i<strlen(zeile) ; i++ ) {
		b = zeile[i];

		switch( b ) {
			case '[':
				if ( f_bild==1 ) {
					f_ein=1;
					ws_nr=0;
			  }
				break;
			case ']':
				if( f_ein ) {
					ws_anz++;
					ws_nr=0;
				}
				f_ein=0;
				break;
			case '|':
				if( f_ein ) {
					ws_anz++;
					ws_nr=0;
				}
				break;
			case '{':
				if( f_bild==0)
					f_bild=1;
				break;
			case '}':
				f_bild=2;
				break;
			}

		if( f_ein ) {
			if( b!=' ' && b!='\t' && b!='[' && b!='|' && ws_nr<8 ) {
				werte_screen[ws_anz][ws_nr]=b;
				ws_nr++;
				werte_screen[ws_anz][ws_nr]='\0';
			}
		}
	}
}

void hole_attrib( char zeile[] ) {
	int     i;
	char    b;
	char    f_gleich=0;

	wa_nr = 0;

	for( i=0 ; i<strlen(zeile) ; i++ ) {
		b = zeile[i];

		switch( b ) {
			case '=':
				if( f_gleich==0 )
					f_gleich=1;
				else
					if( f_gleich==1 ) {
						f_gleich=2;
						wa_anz++;
					}
				break;
			case ';':
			case ',':
				if( f_gleich==1 ) {
					f_gleich=2;
					wa_anz++;
				}
				break;
		}

		if( f_gleich )
			if( b!='\t' && b!=' ' && b!='=' && wa_nr<30)
				{
				werte_attrib[wa_anz][wa_nr]=b;
				wa_nr++;
				werte_attrib[wa_anz][wa_nr]='\0';
				}
	}

	if( f_gleich==2 ) { /* d.h. hier wurde eine tab.row gefunden */
		for( i=0 ; i<strlen(zeile) ; i++ ) {
			b = zeile[i];

			if( b==' ' || b=='\t' || b=='=' )
				zeile[i]='\0';
		}

		strlwr( zeile );

		for( i=0 ; i<ws_anz ; i++ )
			if( finde_cmp( werte_screen[i],zeile ) ) {
				/*
				printf("\t\t%s,%s\n",werte_screen[i],zeile );
				*/
				werte_attrib_nr[i]=wa_anz-1;
			}
	}
}


int finde_cmp( char zeile[],char str[] ) {
	char    zeile2[100];

	strcpy( zeile2,zeile );
	strlwr( zeile2 );

	if( strcmp( zeile2,str )==0 )
		return(1);
	else
		return(0);
}

int finde( char zeile[],char str[] ) {
	char    zeile2[100];

	strcpy( zeile2,zeile );
	strlwr( zeile2 );

	if( strstr( zeile2,str ) )
		return(1);
	else
		return(0);
}

void fehler( char meldung[] ) {
	printf("\n\n%s...\n\n",meldung);
	exit(1);
}

void strlwr( char *str ) {
	int	i;
	
	for( i=0 ; i<strlen(str) ; i++ )
		if( str[i]>='A' && str[i]<='Z' )
			str[i]+=32;
}
