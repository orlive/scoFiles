extern "C" {

#include "eingabe.h"
#include "etc.h"
#include "os.h"

int input(WINDOW *w,int y,int x,int l,char *str) {
  int  t;
  int  tue = TRUE;
  int  pos = 0;
  char  flg_einf = 1;

  keypad(w,TRUE);

  while (tue) {
    str[l] = '\0';
    while (strlen(str)<l)
      strcat(str," ");

    mvwprintw(w,y,x,"%s",str);
    wrefresh(w);

    wmove(w,y,x+pos);
    flg_int = 0;
    t = wget_taste(w);
    if (flg_int)
      t = 27;

    /* mvwprintw(w,0,0,"%3d",t); */

    switch (t) {
      case 8:    /* backspace */
        xdelete(str,pos-1,1);
        pos--;
        break;
      case 24:  /* ^X */
        xdelete(str,pos,1);
        break;
      case 1:    /* ^A */
        flg_einf = 1-flg_einf;
        break;
      case 4:    /* ^D */
        str[pos]='\0';
        break;
      case 27:
        return 0;
      case '\n':
        tue = FALSE;
        break;
      case KEY_LEFT:
        pos--;
        break;
      case KEY_RIGHT:
        pos++;
        break;
      case 360:  /* Ende */
        for (pos=l-1;pos>=0;pos--)
          if (str[pos]!=' ')
            break;
        pos++;
        break;
      case 262:  /* Pos1 */
        pos=0;
        break;
    }
    if ( (t>='a' && t<='z') || (t>='A' && t<='Z') ||
         (t>='0' && t<='9') ||
         strchr("[^_?/\\]-><:;.,* ",t) ) {
      if ( flg_einf ) {
        if (str[strlen(str)-1]==' ') {
          xinsert(str,pos," ");
          str[pos++] = t;
        }
      } else {
        str[pos++] = t;
      }
    }

    if (pos<0)
      pos = 0;
    else if (pos>=l)
      pos = l-1;
  }
  
  for (t=l-1;t>=0;t--)
    if (str[t]!=' ')
      break;
    else
      str[t] = '\0';
  return 1;
}

}

