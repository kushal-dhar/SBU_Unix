#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
char * getenv(char * str){
   int len = strlen(str);
    printf("%s",str);
    if(len == 4){
        if(strstr(str,"PATH") != 0) {
//        if(str[0] == 'p' && str[1] == 'a' && str[2] == 't' && str[3] == 'h' ){
        strcpy(vars.path, "/home/kdhar/:/bin:/usr/bin");
        printf("%s\n",vars.path);
        return vars.path;
        }
   }
   else if (len == 3){
        if(strstr(str,"PS1") != 0) {
//        if(str[0] == 'p' && str[1] == 's' && str[2] == '1'){
        printf("%s\n",vars.ps1);
        return vars.ps1;
        }
   }
   else {
    printf("%s","Wrong variable");
        }
  return 0;
}

char *setenv(char *str) {
  int len = strlen(str);
  char *value;
  char *path_val;
  int i = 0;

  if(strstr(str, "PATH") != 0) {
    while(global_envp[i] != '\0') {
      printf("%s",envp[i]);
      if(strstr(global_envp[i], "PATH") != 0 && vars.path != ' ') {
        strcat(vars.path, global_envp[i]);
        strcat("%s",vars.path, str);
        break;
      }
      else {
        strcat(vars.path, str);
        printf("%s",vars.path);
        break;
      }
      i++;
    }
  }
  else if (str(str,"PS1") != 0) {
      strcat(vars.ps1, str);
      printf("%s",vars.ps1);
  }
  return 0;
}

*/
