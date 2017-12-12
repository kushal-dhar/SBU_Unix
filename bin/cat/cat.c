#include <libc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/defs.h>

void cat(int argc, char *argv[]) {
  int     j          = 0;
  char   *text       =  (char *) mallocc(sizeof(char)*1000);
  uint64_t    fd;

  /* Loops through all files and displays them one by one */
  for (j = 0; j < argc; j++) {
    fd = open(argv[j], 0);
    text[0]='\0';
    read(fd, text,1000);
    printf("%s",text);
  }

  return;
}

int main(int argc, char *argv[], char *envp[]) {
  cat(argc,argv);
  return 0;
}
