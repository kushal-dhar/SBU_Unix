#include <sys/kprintf.h>
/* 
 * Finds the length of string
 */
int strlen(char *str) {
   char * temp = str;
    int i = 0;
    while (*temp != '\0') {
	i++;
	temp++;
    }
    *temp++ = '\0';
    return i;
}

/*
 * Copy string src to string dst
 */
char *strcpy(char *src, char *dst) {
    char *temp = dst;
    char *sr = src;
    while (*sr != '\0') {
	*temp++ = *sr++;
    }
    *temp++ = '\0';
    *temp = '\0';
    return dst;
}

/* 
 * Compares string str1 and str2
 */
int strcmp(char *temp1, char *temp2) {
    char *str1 = temp1;
    char *str2 = temp2;
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

int atoi(char *str) {
    int val = 0;
    while (*str != '\0') {
	val = (val * 10) + (*str - '0');
	str++;
    }
    return val;
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
