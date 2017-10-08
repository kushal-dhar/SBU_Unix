#ifndef _STRING_H_
#define _STRING_H_ 1

extern   char*  strcpy(char *dest, char *src);
extern   char*  strncpy(char *dest, char *src, int n);
extern   int    strlen(char *str);
extern   char*  strcat(char *dst, char *src);
extern   char*  strstr(char *str, char *search);
extern char ** split_string(char str[], char delim, char **s);
#endif
