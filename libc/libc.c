#include <stdio.h>
#include <libc.h>

char global_var[1000];
char* read_sys(long syscall, long arg1, long arg2, long arg3) {
 long ret;
  __asm__ volatile (
    "movq %1, %%rax\n\t"
    "movq %2, %%rdi\n\t"
    "movq %3, %%rsi\n\t"
    "movq %4, %%rdx\n\t"
    "syscall\n\t"
    "movq %%rax, %0\n\t"
    :"=r"(ret)
    : "a" (syscall), "b" (arg1), "c" (arg2), "d" (arg3)
    : "memory");
  return (char *) ret;
}

long write_sys(long syscall, long arg1, long arg2, long arg3) {
  long ret; 
  __asm__ volatile (
    "movq %1, %%rax\n\t"
    "movq %2, %%rdi\n\t"
    "movq %3, %%rsi\n\t"
    "movq %4, %%rdx\n\t"
    "syscall\n\t"
    "movq %%rax, %0\n\t"
    :"=a"(ret)
    : "a" (syscall), "b" (arg1), "c" (arg2), "d" (arg3)
    : "memory");
  return ret;
} 


inline  long syscall_wait(long syscall, long arg1, int *arg2, long arg3) {
  long pid;
  __asm__ volatile (
    "movq %1, %%rax\n\t"
    "movq %2, %%rdi\n\t"
    "movq %3, %%rsi\n\t"
    "movq %4, %%rdx\n\t"
    "syscall\n\t"
    "movq %%rax, %0\n\t"
    :"=c"(pid)
    : "a" (syscall), "b" (arg1), "c" (arg2), "d" (arg3)
    : "memory");
  return pid;
}

inline long syscall_open(long syscall, long arg1, long arg2) {
  long ret;

  __asm__ volatile (
    "movq %1, %%rax\n\t"
    "movq %2, %%rdi\n\t"
    "movq %3, %%rsi\n\t"
    "syscall\n\t"
    "movq %%rax, %0\n\t"
    :"=a"(ret)
    : "a" (syscall), "b" (arg1), "c" (arg2)
    : "memory");
  return ret;
}

inline long syscall_dup(long syscall, long arg1, long arg2) {
  long ret;

  __asm__ volatile (
    "movq %1, %%rax\n\t"
    "movq %2, %%rdi\n\t"
    "movq %3, %%rsi\n\t"
    "syscall\n\t"
    "movq %%rax, %0\n\t"
    :"=a"(ret)
    : "a" (syscall), "b" (arg1), "c" (arg2)
    : "memory");
  return ret;
}

inline long syscall_close(long syscall, long arg1) {
  long ret;

  __asm__ volatile (
    "movq %1, %%rax\n\t"
    "movq %2, %%rdi\n\t"
    "syscall\n\t"
    "movq %%rax, %0\n\t"
    :"=a"(ret)
    : "a" (syscall), "b" (arg1)
    : "memory");
  return ret;
}

inline long syscall_pipe(long syscall, long* arg1) {
  long ret;

  __asm__ volatile (
    "movq %1, %%rax\n\t"
    "movq %2, %%rdi\n\t"
    "syscall\n\t"
    "movq %%rax, %0\n\t"
    :"=a"(ret)
    : "a" (syscall), "b" (arg1)
    : "memory");
  return ret;
}

inline long syscall_fork(long syscall, long arg1) {
  long ret;

  __asm__ volatile (
    "movq %1, %%rax\n\t"
    "movq %2, %%rdi\n\t"
    "syscall\n\t"
    "movq %%rax, %0\n\t"
    :"=a"(ret)
    : "a" (syscall), "b" (arg1)
    : "memory");
  return ret;
}


inline unsigned int syscall_execve(long syscall, char *file, char *argv[]) {
  long ret;
  __asm__ volatile (
      "mov %1, %%rax\n\t"
      "mov %2, %%rdi\n\t"
      "mov %3, %%rsi\n\t"
      "mov %4, %%rdx\n\t"
      "syscall\n\t"
      "mov %%rax, %0\n\t"
    :"=a"(ret)
    : "a" (syscall), "b" (file), "c" (argv), "d" (global_envp)
    : "memory");
  return ret;

}


inline long syscall_dirent(long syscall, long fd, char *buf, long count){
  long ret;
  int  i = 0;

  while(i<1000) {
    ls_str[i++] = '\0';
  }
  __asm__ volatile (
    "mov %1, %%rax\n\t"
    "mov %2, %%rdi\n\t"
    "mov %3, %%rsi\n\t"
    "mov %4, %%rdx\n\t"
    "syscall\n\t"
    "mov %%rax, %0\n\t"
    :"=a"(ret)
    : "a" (syscall), "b" (fd), "c" (global_str), "d" (count)
    : "memory");

  i = 0;
  while ( global_str[i]!= '\0') {
    ls_str[i] = global_str[i];
    i++;
  }

  return ret;

}

long write_char(long syscall, long arg1, char *arg2, long arg3) {
  long ret;
  int i = 0;

  while (*arg2 != '\0') {
    global_str[i++] = *arg2;
    arg2++;
  }
  global_str[i] = '\0';

  __asm__ volatile (
    "movq %1, %%rax\n\t"
    "movq %2, %%rdi\n\t"
    "movq %3, %%rsi\n\t"
    "movq %4, %%rdx\n\t"
    "syscall\n\t"
    "movq %%rax, %0\n\t"
    :"=a"(ret)
    : "a" (syscall), "b" (arg1), "c" (arg2), "d" (arg3)
    : "memory");
  return ret;
}

inline char* read_char(long syscall, long arg1, char *arg2, long arg3) {
  char *ret;
  __asm__ volatile (
    "mov %1, %%rax\n\t"
    "mov %2, %%rdi\n\t"
    "mov %3, %%rsi\n\t"
    "mov %4, %%rdx\n\t"
    "syscall\n\t"
    "mov %%rax, %0\n\t"
    :"=c"(ret)
    : "a" (syscall), "b" (arg1), "c" (global_str), "d" (arg3)
    : "memory");
  ret = global_str;
  return ret;
}

