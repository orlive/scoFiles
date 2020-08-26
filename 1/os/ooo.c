#include <stdio.h>
#include <stdlib.h>

main(int argc,char **argv)
{
	char	tmp[100];

	if( argc!=3 )
		exit(0);

	if( !strcmp(argv[1],"0") )
	{
		/* entschlüsseln */
		sprintf(tmp,"mv S.%s %s.Z >/dev/null",argv[2],argv[2]);
		(void)system(tmp);
		sprintf(tmp,"uncompress %s.Z >/dev/null",argv[2]);
		(void)system(tmp);
	}
	if( !strcmp(argv[1],"1") )
	{
		/* verschlüsseln */
		sprintf(tmp,"compress %s >/dev/null",argv[2]);
		(void)system(tmp);
		sprintf(tmp,"mv %s.Z S.%s >/dev/null",argv[2],argv[2]);
		(void)system(tmp);
	}
}
