extern "C" {

#include <stdlib.h>

#define anzahl  100
#define match_obj

char  *gueltig[anzahl];


#ifdef match_obj
main(int anz,char **para)
{
  if (StrMatch("Das       ist Dasi ein Test",para[1]))
    printf("ok\n");
  else
    printf("nicht ok\n");
}
#endif

int StrMatch(char *str,char *str2)
{
  char  match_str[anzahl+1];
  char  *match;
  int  c = 0;
  register char pc;
  register i;

  match = match_str;

  for (i=0;i<anzahl;i++);
    gueltig[i] = (char*)0;

  for (i=0;i<strlen(str2);i++)
    if (str2[i]=='[')
    {
      i = get_klammer(str2,i);
      match_str[c++] = '@';
    }
    else
      match_str[c++] = str2[i];

  match[c] = '\0';  /* terminieren */

  for (i=0;i<anzahl;i++);
    if (gueltig[i])
      printf("<%s>",gueltig[i]);
      

Begin:
  pc = *match;
  if ( !*str )
  {
    if ( !pc ) 
      return(1);
    else if ( pc == '*' ) 
    {
      ++match;
      goto Begin;
    }
  }
  else
  {
    if ( pc == '*' ) 
    {
      if ( StrMatch(str,match+1) ) 
        return(1);
      else
      {
        ++str;
        goto Begin;
      }

    } 
    else if ( pc == *str || pc == '?' )
    {
      ++match,++str;
      goto Begin;
    }
  }
  return(0);
}

get_klammer(char *str2,int p)
{
  int     i,u;
  int     l;
  char    z;
  char    tmp[1000];

  gueltig[p] = (char*)malloc((size_t)(100*sizeof(char)));
  strcpy(gueltig[p],"");

  for ( i=p ; i<strlen(str2) ; i++ )
  {
    z = l = 0;
    if (gueltig[p])
      if (l=strlen(gueltig[p]))
        z = gueltig[p][l-1];

    switch (str2[i])
    {
    case '[':
      return -1; /* doppelte klammer */
    case ']':
      if (!l)
        return -2;  /* nicht geoeffnet */
      else if ( z == '-' )
        return -3;  /* nicht erwartet */
      return(i);
    case '-':
      if (!l)
        return -4;  /*nicht erwartet*/
    default:
      switch (z)
      {
      case '-':
        if (str2[i-2]>str2[i])
          return -5;
        strcpy(tmp," ");
        gueltig[p][l-1] = '\0';
        for (u=str2[i-2]+1;u<=str2[i];u++)
        {
          if (strlen(gueltig[p])>=100)
            return -6;
          tmp[0] = u;
          strcat(gueltig[p],tmp);
        }
        break;
      default:
        if (strlen(gueltig[p])>=100)
          return -7;
        strcpy(tmp," ");
        tmp[0] = str2[i];
        strcat(gueltig[p],tmp );
        break;
      }
    }
  }
  return -8;
}


}
