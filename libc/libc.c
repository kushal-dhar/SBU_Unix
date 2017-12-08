#include <stdio.h>
#include <libc.h>

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

uint64_t syscall_getpid(uint64_t syscall) {
    uint64_t ret = 0;

  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("int $0x80;");
  __asm__ volatile ("movq %%rax, %0;":"=r"(ret));

    return ret;
}

void syscall_1(uint64_t syscall) {
  uint64_t  ret = 0;

  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("int $0x80;");
  __asm__ volatile ("movq %%rax, %0;":"=r"(ret));
//  ret = global_val;
//  return ret;
}

void syscall_2(uint64_t syscall, uint64_t ch) {
//  uint64_t  ret = 0;
   
  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("movq %0, %%rcx;"::"r"(ch));
  __asm__ volatile ("movq %0, %%r10;"::"r"(&global_val));
  __asm__ volatile ("int $0x80;");
//  __asm__ volatile ("movq %%rax, %0;":"=r"(ret)); 
//  ret = global_val;
//  return ret;
}

void syscall_2_1(uint64_t syscall, uint64_t ch) {

  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("movq %0, %%rcx;"::"r"(ch));
  __asm__ volatile ("movq %0, %%r10;"::"r"(&global_val));
  __asm__ volatile ("int $0x80;");
}


void syscall_3(uint64_t syscall, uint64_t ch, uint64_t buf) {
//  uint64_t ret = 0;

  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("movq %0, %%rcx;"::"r"(ch));
  __asm__ volatile ("movq %0, %%rdx;"::"r"(buf));
  __asm__ volatile ("movq %0, %%r10;"::"r"(&global_val));
  __asm__ volatile ("int $0x80;");
//  __asm__ volatile ("movq %%rax, %0;":"=r"(ret));
//  ret = global_val;

//  return ret;
}

void syscall_4(uint64_t syscall, uint64_t ch, uint64_t buf, uint64_t size) {
//  uint64_t ret = 0;

  __asm__ volatile ("movq %0, %%rax;"::"r"(size));
  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("movq %0, %%rcx;"::"r"(ch));
  __asm__ volatile ("movq %0, %%rdx;"::"r"(buf));
  __asm__ volatile ("int $0x80;");
//  __asm__ volatile ("movq %%rax, %0;":"=r"(ret));

//  return ret;
}

void syscall_2next(uint64_t syscall) {
  uint64_t  ret = 0;
  char * temp = sys_temp;
  uint64_t* ch = (uint64_t *) temp;
  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("movq %0, %%rcx;"::"r"(ch));
  __asm__ volatile ("movq %0, %%r10;"::"r"(&global_val));
  __asm__ volatile ("int $0x80;");
  __asm__ volatile ("movq %%rax, %0;":"=r"(ret));
//  ret = global_val;
//  return ret;
}

void syscall_2_char(uint64_t syscall) {
//  uint64_t  ret = 0;

  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("movq %0, %%rcx;"::"r"(global_str));
  __asm__ volatile ("movq %0, %%r10;"::"r"(&global_val));
  __asm__ volatile ("int $0x80;");
//  __asm__ volatile ("movq %%rax, %0;":"=r"(ret));
//  ret = global_val;
//  return ret;
}


