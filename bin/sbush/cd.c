#include <unistd.h>
#include <string.h>
#include <stdio.h>
void change_dir(char *cmd) {
  char gwd[1024];
  int i = 0;

  cmd += 3;
  cmd[strlen(cmd) - 1] = '\0';
  while(*cmd != '\0') {
    if (*cmd != ' ' || *cmd != '\t') {
      gwd[i++] = *cmd;
    }
    cmd++;
  }
  gwd[i] = '\0';
  if (chdir(gwd) < 0) {
  printf("%s", "Wrong address :Directory is not changed");
  }
}
