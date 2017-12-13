#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libc.h>


int  stringstr(char *str, char *search) {
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
      printf("Hello here\n");
      return 1;
    }
  }


  return 0;;
}


int main(int argc, char *argv[], char *envp[]) {
#if 0
    char str[5][50];
    char temp_str[50];
    int i = 0;
    int j = 0;
    char *str1;
    while(j < argc) {
	str1 = (char *)argv+(j*64);
	i = 0;
        while (*str1 != '\0') {
	    temp_str[i++] = *(char *)str1;
	    str1++;
        }
        temp_str[i] = '\0';
        strcpy(str[j], temp_str);
	j++;
	i = 0;
	while(temp_str[i] != '\0') {
	    temp_str[i++] = '\0';
	}
    }
#endif
//    cwd();
    char *str = (char *)mallocc(sizeof(char)*4000);
    char *temp = str;
    strcpy(str, "Hello world, how you doing!!\n");
    printf("before str\n");
    stringstr(str, "world");
        printf("str: %s",temp);
    printf("str1: %s",temp);
/*    char *str1 = (char *)mallocc(sizeof(char)*400000);
    strcpy(str1, "This is Joey!!\n");
    printf("str: %s",str1);
    printf("Argc here:%d\n ",argc);
    for (int i = 0; i < argc; i++) {
	printf("Arg: %s\n",argv[i]);
    }*/
//    exit();
    while(1);
    return 0;
}
