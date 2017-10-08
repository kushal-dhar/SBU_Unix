#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
      printf("\n%s", fd->f_name);

      j += fd->f_reclen;
    }
  }
  return 0;
}
