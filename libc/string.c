#include <sys/defs.h>
#include <stdio.h>

char* strcpy(char *dest, char *src) {
  char *tmp = dest;
  char *temp2 = src;
  while(*temp2 != '\0') {
    *tmp++ = *temp2++;
  }
  *tmp++ = '\0';
  *tmp ='\0';
  return (char*)dest;
}


char* strncpy(char *dest, char *src, int n) {
  char *tmp = dest;
  char *temp2 = src;
  int   i   = 0;

  while(*temp2 != '\0' && i++ < n) {
    *tmp++ = *temp2++;
  }
  *tmp++ = '\0';
  *tmp = '\0';
  return dest;
}


int strlen(char *str) {
  int len = 0;
   char *temp = str;
  while(*temp++ != '\0') {
    len++;
  }
  *temp++ = '\0';
   *temp = '\0';
  return len;
}


char *strcat(char *dst, char *src) {
  char *tmp = dst;
  char *sr = src;
  while(*tmp != '\0') {
      tmp++;
  }
  while(*sr != '\0') {
      *tmp++ = *sr++;
  }
  *tmp++ = '\0';
  *tmp = '\0';
  return dst;
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

uint64_t strstr(char *str, char *search) {
  int len1, len2;
  int i = 0, j = 0;
  int  found = 0;
//  char *tmp;

  len1 = strlen(str);
  len2 = strlen(search);

  if (len2 > len1) {
    return 0;
  }
  if ( !len1 || !len2) {
    return 0;
  }

  for (i = 0; i < len1 - len2; i++) {
    found = 1;
    for (j = i; j < i + len2; j++) {
      if (str[j] != search[j-i]) {
	found = 0;
	break;
      }
    }
    if (found == 1) {
      found = 1;
      return 1;
    }
  }

  return 0;;
}

/*
 * Compares string str1 and str2
 */
int strcmp(char *temp1, char *temp2) {
   char * str1 = temp1;
   char * str2 = temp2;
    while (*str1 != '\0' && *str2 != '\0') {
        if (*str1 != *str2) {
            return 1;
        }
        str1++;
        str2++;
    }
    if (*str1 != '\0' || *str2 != '\0') {
        return 1;
    }
    return 0;
}

void* memset(void *ptr, int x, uint32_t n) {
    char *temp = (char*)ptr;
    while(n > 0) {
        *temp = x;
        temp++;
        n --;
    }
    return ptr;
}

/*
void* memset(void *ptr, int x, uint32_t n) {
    char *temp = ptr;
    while(n > 0) {
        *temp = x;
        temp++;
        n --;
    }
    return ptr;
} */

int  substr_tillchar(char *s, char c){
    char *temp = s;
    int len = strlen(temp);
    int i = len;
    int count = 0;
    while(count < 2  && i >= 0 ){
        if(*(temp+i) == c){ count++;}
        i--;
    }
    if (i < 0){
    s[0]='\0';
    return -1;
    }
    i = i+2;
    *(temp+i) =  '\0';
    *(temp+i+1) =  '\0';
    return 0;
}

int atoi(char *str) {
    int val = 0;
    while (*str != '\0') {
        val = (val * 10) + (*str - '0');
        str++;
    }
    return val;
}

void addDelimiter(char *str){
    char *temp = str;
   while(*temp != '\0'){
        if(*temp == ' '){
             *temp = '\n';
        }
        temp++;
   }
    *temp++ = '\n';
    *temp ='\0';
}

void removespace(char * str){
char *temp = str;
while(*temp != '\0' && *temp != ' ') temp++;
if(*temp == ' '){
  temp++;
  char *ch = temp;
  while(*ch != '\0' && *ch == ' ') ch++;
  if(*ch != '\0'){
  strcpy(temp,ch);
  }  
}
}
