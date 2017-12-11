#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/defs.h>


void ls(int argc, char *argv[]) {
  int     j          = 0;
  char   *text       =  (char *) mallocc(sizeof(char)*1000);
  uint64_t     fd;

  printf("hello I am in ls");
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

  while(1);
  return;
}

int main(int argc, char *argv[], char *envp[]) {
  ls(argc,argv);
  return 0;
}

#if 0
char str[1000];
char buf[1000];
char cmd[1000];

struct f_dirent  {
  unsigned long  f_ino;     /* Inode number */
  unsigned long  f_off;     /* Offset to next linux_dirent */
  unsigned short f_reclen;  /* Length of this linux_dirent */
  char           f_name[];  /* Filename (null-terminated) */
};
inline long list_dir(long syscall, long fd, char  *buf ,long count) {
   long ret;
  __asm__ volatile (
    "mov %1, %%rax\n\t"
    "mov %2, %%rdi\n\t"
    "mov %3, %%rsi\n\t"
    "mov %4, %%rdx\n\t"
    "syscall\n\t"
    "mov %%rax, %0\n\t"
    :"=a"(ret)
    : "a" (syscall), "b" (fd), "c" (buf), "d" (count)
    : "memory");

   return ret;
}

int main(int argc, char *argv[], char *envp[]) {
  int i = 0;
  int j = 0;
  struct f_dirent *fd;
  int n;

  for( i = 0; i < argc; i++) {
    n=open(argv[i+1],4);
    n = list_dir(217,n,buf,1000);
    for (j = 0; j < n;) {
      fd = (struct f_dirent *) (buf + j);
      //printf("\n%s", fd->f_name);

      j += fd->f_reclen;
    }
  }
  return 0;
}
#endif
