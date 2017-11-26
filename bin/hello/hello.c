#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
/*    char str[100];
    scan("%s",(void *)str);
    print("%s\n",str);
    scan("%S",(void *)str);
    print("%s\n",str); */
    
    int fd = open_dir("lib/");
    if (fd == -1) {
	print("Not a valid directory\n");
    }
    print("Working %d\n",fd);
    read_dir(fd);
    fd = open("bin/hello",3);
    print("File number: %d\n",fd);
/*  uint64_t syscall = 39, ret;
  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("int $0x80;");
  __asm__ volatile ("movq %%rax, %0;":"=r"(ret));
   return ret; */
    while(1);
    return 0;
}
