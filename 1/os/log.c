#include <stdio.h>
#include <stdlib.h>

FILE* logFD;
int	err_cnt;

int log_open()
{
	char	*ptr;

	if( (ptr = (char*)getenv("LOG"))==NULL)
		return 0;
	
	if( (logFD = fopen(ptr,"a"))==NULL)
		return 0;

	return 1;
}

void log_close()
{
	fclose(logFD);
}

void log_print(char* s1)
{
	if(log_open())
	{
		fprintf(logFD,"%s\n",s1);
		log_close();
	}
}

/*----debugging functions----*/

void debug_print()
{
	char	t[20];

	sprintf(t,"debug: %d",++err_cnt);

	log_print(t);
}

void debug_clear()
{
	err_cnt = 0;
}
