#include <unistd.h>
#include <string.h>
#include <sys/defs.h>
#include <stdio.h>
void checkForBinary(char str[], char delim){
char *s[100];
char str2[100] = "/bin/";
strcat(str2,str);
split_string(str2,delim,s);
int i= -1;
int status=0;
if((i = fork()) == 0){
  execvp(s[0], s);
}else{
waitpid(i,&status,0);
return;
}
}

