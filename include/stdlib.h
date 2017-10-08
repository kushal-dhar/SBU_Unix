#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/defs.h>

int main(int argc, char *argv[], char *envp[]);
void exit(int status);

void *malloc(size_t size);
void free(void *ptr);
//long mmap(unsigned long syscall, unsigned long arg1, unsigned long arg2,
//unsigned long arg3, unsigned long arg4, unsigned long arg5, unsigned long arg6);
#endif
