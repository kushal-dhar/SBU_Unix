#include <stdlib.h>
void exit(int status){
long syscall = 60;
__asm__ volatile (
    "mov %0, %%rax\n\t"
    "mov %1, %%rdi\n\t"
    "syscall\n\t"
    :
    : "r"(syscall), "r"((uint64_t)status)
);
}
