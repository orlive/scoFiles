#include <stdio.h>
#include <curses.h>
#include <string.h>
#include "tetris.h"

input(w,y,x,l,str)
	WINDOW	*w;
	int	y,x,l;
	char	*str;
{
	int	t;
	int	tue = TRUE;
	int	pos = 0;

	str[l] = '\0';
	while(strlen(str)<l)
		strcat(str," ");

	nodelay(w,TRUE);
	keypad(w,TRUE);

	while(tue)
	{
		mvwprintw(w,y,x,"%s",str);
		wrefresh(w);

		while((t=mvwgetch(w,y,x+pos))==EOF);

		switch(t)
		{
			case '\n':
				tue = FALSE;
				break;
			case KEY_LEFT:
				pos--;
				break;
			case KEY_RIGHT:
				pos++;
				break;
			default:
				if( (t>='a' && t<='z') || (t>='A' && t<='Z')
						|| (t>='0' && t<='9') 
						|| strchr(":;., ",t) )
					str[pos++] = t;
		}

		if(pos<0)
			pos = 0;
		else if(pos>=l)
			pos = l-1;
	}
	
}
