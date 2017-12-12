#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/defs.h>


void ls(int argc, char *argv[]) {
  int     j          = 0;
  char   *text       =  (char *) mallocc(sizeof(char)*1000);
  uint64_t     fd;

  /* Loops through all files and displays them one by one */
  for (j = 0; j < argc; j++) {
    fd = open_dir(argv[j]);
    if(fd == 999){
       break; 
    }
    text[0]='\0';
    read_dir(fd);
    printf("%s",text);
  }
  return;
}


int main(int argc, char *argv[], char *envp[]) {
  ls(argc,argv);
  return 0;
}
