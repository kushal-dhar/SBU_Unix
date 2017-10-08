#include <string.h>
char* strcpy(char *dest, char *src) {
  char *tmp = dest;

  while(*src != '\0') {
    *dest++ = *src++;
  }
  *dest = '\0';
  return tmp;
}


char* strncpy(char *dest, char *src, int n) {
  char *tmp = dest;
  int   i   = 0;

  while(*src != '\0' && i++ < n) {
    *dest++ = *src++;
  }
  *dest = '\0';
  return tmp;
}


int strlen(char *str) {
  int len = 0;

  while(*str++ != '\0') {
    len++;
  }
  return len;
}


char *strcat(char *dst, char *src) {
  char *tmp = dst;
  while(*dst != '\0') {
      dst++;
  }
  while(*src != '\0') {
      *dst++ = *src++;
  }
  *dst = '\0';
  return tmp;
}

char ** split_string(char str[], char delim, char **s){
    int i, j;
    i = j = 0;
    while(str[j] != '\0')
    {
        while(str[j] == delim )
        {
            str[j] = '\0';
            j++;
        }
        if(str[j] !='\0' )
        {
            s[i] = str + j;
            while(((str[j]) !=delim) && (str[j]!='\0'))
            {
                j++;
            }
            i++;
        }
    }
    s[i] = 0;
    return s;
}

char* strstr(char *str, char *search) {
  int len1, len2;
  int i = 0, j = 0;
  int  found = 0;
  char *tmp;

  len1 = strlen(str);
  len2 = strlen(search);

  if (len2 > len1) {
    return 0;
  }
  if ( !len1 || !len2) {
    return 0;
  }
  while (i < len1 && j < len2) {
    if (search[j] == str[i]) {
      if (found == 0) {
        tmp = str+i;
      }
      i++;
      j++;
      found = 1;
    }
    else if (found == 1) {
      j = 0;
      i++;
      found = 0;
    }
    else {
      i++;
    }
  }

  if (found == 1) {
    return tmp;
  }
  return 0;;
}

