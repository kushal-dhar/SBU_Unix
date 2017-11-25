#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
    print("Hello world %d",2);
    print("Working\n");
/*  uint64_t syscall = 39, ret;
  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("int $0x80;");
  __asm__ volatile ("movq %%rax, %0;":"=r"(ret));
   return ret; */
    while(1);
    return 0;
}
