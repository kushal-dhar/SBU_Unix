#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void  _start(){
  uint64_t *rsp = 0;
  uint64_t *argc = 0;
  char **argv = NULL;
<<<<<<< HEAD
//  char **new_argv = (char **)mallocc(sizeof(char*)*5);
=======
>>>>>>> 6d30c471091394f8d1fbe95cb091434d57e48294
  char **envp = NULL;

  __asm__ volatile(
    "movq %%rsp, %0;"
    : "=a"(rsp)
    :
    :"memory"
  );

  argc = ((uint64_t *)rsp+1);
  argv = ((char **)rsp + 2);
<<<<<<< HEAD
//  new_argv[0] = argv;
  if(*argc > 1) {
    envp = argv + (*argc + 1);
  }
=======
/*  }
>>>>>>> 6d30c471091394f8d1fbe95cb091434d57e48294
  else {
    envp = argv + 2;
  }

  global_envp = envp; */
  main(*argc, argv, envp);
  exit(1);
}

