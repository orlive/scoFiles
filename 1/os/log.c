extern "C" {

#include "log.h"

static FILE* logFD;
int	   err_cnt;

#define FILE_NAME "log.log"

int log_open() {
	static char *logFileName;
	char	*ptr;

	if ( !logFileName && getenv("OS") ) {
		logFileName = (char*) malloc( strlen(getenv("OS")) + strlen(FILE_NAME) + 2 );
		sprintf(logFileName,"%s/%s",getenv("OS"),FILE_NAME );
	}
	
	if ( logFileName ) {
		logFD = fopen(logFileName,"at");
	}

	return logFD ? 1 : 0;
}

void log_close() {
	fclose(logFD);
}

void logPrintf(const char *callerFile,int callerLine,const char *msg, ...) {
	if ( log_open() ) {
		va_list params;
		
		va_start( params, msg );
		fprintf( logFD ,"%s/%d: ",callerFile,callerLine);
		vfprintf( logFD , msg , params );
		fprintf( logFD,"\n" );
		va_end( params );

		log_close();
	}
}

void log_print(char* s1) {
	if ( log_open() ) {
		fprintf(logFD,"%s\n",s1);
		log_close();
	}
}

/*----debugging functions----*/

void debug_print() {
	char	t[20];

	sprintf(t,"debug: %d",++err_cnt);

	log_print(t);
}

void debug_clear() {
	err_cnt = 0;
}

}
