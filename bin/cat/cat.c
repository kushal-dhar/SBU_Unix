#include <libc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/defs.h>

void cat(int argc, char *argv[]) {
  int     j          = 0;
  char   *text       = NULL;
  int     fd;


  /* Loops through all files and displays them one by one */
  for (j = 0; j < argc; j++) {
    fd = open(argv[j], 2);
    text = read_char(0, fd, text, 1000);
    printf("%s",text);
  }

  return;
}

int main(int argc, char *argv[], char *envp[]) {
  cat(argc,argv);
  return 0;
}
