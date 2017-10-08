#ifndef _STDIO_H
#define _STDIO_H

#define STDIN     0
#define STDOUT    1
#define STDERR    2

static const int EOF = -1;
char **global_envp;

int putchar(int c);
int puts(const char *s);
//int printf(const char *format, ...);
char* fgets(char *dst, int max, long fp);
char *gets(char *s);
char getchar();
void cat(int argc, char *argv[]);
//void cat(char *cmd);
//void list_dir(char *cmd);
void printf(char s[],char  str[]);
extern void printInt(int i );
extern long list_dir(long syscall, long fd, char  *buf ,long count);
#endif

