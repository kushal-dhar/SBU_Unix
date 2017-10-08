#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "get_cmd.h"

/*
char* get_cmd(char *cmd) {
  char env_var[1024];
  char *var;
  int i = 0;

  while(*cmd != '\0') {
    if(*cmd != '"' && *cmd != '$' && *cmd != '\t' && *cmd != ' ' && *cmd != '\n') {
      env_var[i++] = *cmd;
    }
    cmd++;
  }
  env_var[i] = '\0';
  if(getenv(env_var)) {
    var = getenv(env_var);
    printf("%s: %s\n",env_var,var);
  } else {
     var = NULL;
     printf("\n");
  }
  return var;
}*/

