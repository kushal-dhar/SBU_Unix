#include <libc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/defs.h>

void echo(int argc, char *argv[]) {
  /* Loops through all ECHO var to search the variable */
  for (int j = 0; j < argc; j++) {
     if( argv[j][0] == '$'){
       char *s = argv[j];
       s++;
       printEchoVar(s);
     }
     else {
       int len =strlen(argv[j]);
       char *ss = argv[j];
       if (*ss == '\'' || *ss == '\"')
       {         ss++;
                 *(ss+len-2)='\0';
       }
       printf(ss);
    }
  }
  return;
}

int main(int argc, char *argv[], char *envp[]) {
  echo(argc,argv);
  return 0;
}
