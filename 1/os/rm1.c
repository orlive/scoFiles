#include <stdio.h>
#include <string.h>

extern void xdelete(char *str,int pos,int anz);
extern void xinsert(char *str1,int pos,char *str2);

int main()
{
	int	i;
	char	str[1000];

	strcpy(str,"12345678901234567890");
		printf("<%s>\tlen:%ld\n",str,strlen(str));
	xdelete(str,19,1);
		printf("<%s>\tlen:%ld\n",str,strlen(str));
	for(i=9;i>-1;i--)
	{
		xdelete(str,i,1);
			printf("%2d) <%s>\tlen:%ld\n",i,str,strlen(str));
	}
	xinsert(str,0,"test");
		printf("<%s>\tlen:%ld\n",str,strlen(str));
	xinsert(str,strlen(str),"test");
		printf("<%s>\tlen:%ld\n",str,strlen(str));
}

void os_uninit() {}

