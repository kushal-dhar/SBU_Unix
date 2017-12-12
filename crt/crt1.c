#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void  _start(){
  uint64_t *rsp = 0;
  uint64_t *argc = 0;
  char **argv = NULL;
  char **envp = NULL;
  char str[5][50];
  char temp_str[50];
  char *str1;
  int i = 0;
  int j = 0;

  __asm__ volatile(
    "movq %%rsp, %0;"
    : "=a"(rsp)
    :
    :"memory"
  ); 
 
  rsp = (uint64_t *)((char*)rsp + 336);

  argc = ((uint64_t *)rsp+1);
  argv = ((char **)rsp + 2);
  while(j < *argc) {
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
  
  char **str2 = (char **)mallocc(sizeof(char *) * 5);
  for (i = 0 ; i < *argc; i++) {
    str2[i] = (char *)mallocc(sizeof(char) * 50);
    strcpy(str2[i], str[i]);
  }

  main(*argc, str2, envp);
  exit(1);
}

