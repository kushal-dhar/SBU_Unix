#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void  _start(){
  uint64_t *rsp = 0;
  uint64_t *argc = 0;
  char *argv = NULL;
  char **new_argv = (char **)mallocc(sizeof(char*)*5);
  char **envp = NULL;

  __asm__ volatile(
    "movq %%rsp, %0;"
    : "=a"(rsp)
    :
    :"memory"
  );

  argc = ((uint64_t *)rsp+1);
  argv = ((char *)rsp + 2);
  new_argv[0] = argv;
  if(*argc > 1) {
    envp = argv + (*argc + 1);
  }
  else {
    envp = argv + 2;
  }

  global_envp = envp;
  main(*argc, argv, envp);
  exit(1);
}

