#include <libc.h>
#include <sys/defs.h>
char* getcwd(char * str, long size){
long syscall = 79;
long ret ;
__asm__ volatile (
    "mov %1, %%rax\n\t"
    "mov %2, %%rdi\n\t"
    "mov %3, %%rsi\n\t"
    "syscall\n\t"
    "mov %%rax, %0\n\t"
    : "=r"(ret)
    : "a"(syscall), "b"(str), "c" (size)
    : "rdi", "rsi"

);
return(char *) ret;
}

int chdir ( char * str){
long syscall = 80;
long  ret ;
__asm__ volatile (
    "syscall\n\t"
    "mov %%rax, %0\n\t"
    : "=a"(ret)
    : "r"(syscall), "r"(str)
    : "rdi"
);
return  (int)ret;
}

int waitpid(int pid, int *wstatus, int options) {

   /* System call for wait is 61*/
   pid = syscall_wait(61, pid, wstatus, options);
   return pid;
}

int execvp(char *file, char *argv[]) {
   int ret;

   /* System call for execve is 59 */
   ret = syscall_execve(59, file, argv);
   return ret;
}

pid_t fork() {
  pid_t ret = 0;

  /* System call for fork is 57 */
  ret = syscall_fork(57, ret);
  return ret;
}

int open(const char *path, int flag) {
  int ret;

  /* 2 is the system call for open */
  ret = syscall_open(2, (long)path, flag);
  return ret;
}

int pipe(int fd[]) {
  int ret;

  /* System call for pipe is 22 */
  ret = syscall_pipe(22,(long*)fd);
  return ret;
}

int close(int fd) {
  int ret;

  /* 3 is the syscall # for close */
  ret = syscall_close(3, fd);
  return ret;
}

int dup2(int oldfd, int newfd) {
   int fd;

   /* System call for dup2 is 33 */
   fd = syscall_dup(33, (long)oldfd, (long)newfd);
   return fd;
}


