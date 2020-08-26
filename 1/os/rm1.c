main()
{
	int	i;
	char	str[1000];

	strcpy(str,"12345678901234567890");
		printf("<%s>\tlen:%d\n",str,strlen(str));
	xdelete(str,19,1);
		printf("<%s>\tlen:%d\n",str,strlen(str));
	for(i=9;i>-1;i--)
	{
		xdelete(str,i,1);
			printf("%2d) <%s>\tlen:%d\n",i,str,strlen(str));
	}
	xinsert(str,0,"test");
		printf("<%s>\tlen:%d\n",str,strlen(str));
	xinsert(str,strlen(str),"test");
		printf("<%s>\tlen:%d\n",str,strlen(str));
}

os_uninit() {}

