#include <stdio.h>
#include <string.h>

int finde(char *s1,char *s2) {
  int len = strlen(s2);

  for (char* c1=s1;*c1;c1++)
    if (!memcmp(c1,s2,len))
      return((int)(c1-s1));
          
  return -1;
}

