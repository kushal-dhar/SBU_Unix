#include <libc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/defs.h>

void echo(int argc, char *argv[]) {
  /* Loops through all ECHO var to search the variable */
  char temp[100];
  for(int i =0; i <100;i++) temp[i]='\0';
  for (int j = 0; j < argc; j++) {
     if( argv[j][0] == '$'){
       char *s = argv[j];
       s++;
       printEchoVar(s);
     }
     else {
       int len =strlen(argv[j]);
       strcpy(temp,argv[j]);
       char *ss = temp;
       if (*ss == '\'' || *ss == '\"')
       {         ss++;
                 *(ss+len-2)='\0';
       }
       printf(ss);
       printf("%s","\n");
    }
  }
  return;
}

int main(int argc, char *argv[], char *envp[]) {
  echo(argc,argv);
  return 0;
}
