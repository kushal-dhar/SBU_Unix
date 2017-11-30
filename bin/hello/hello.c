#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
/*    char str[100];
    scan("%s",(void *)str);
    print("%s\n",str);
    scan("%S",(void *)str);
    print("%s\n",str); */
    
#if 0
    int fd = open_dir("bin/");
    if (fd == -1) {
	print("Not a valid directory\n");
    }
    print("Working %d\n",fd);
    read_dir(fd);
#endif

#if 0
    int fd = open("bin/sbush",3);
    if (fd == -1) {	
	print("Not a valid file\n");
    }
    print("File number: %d\n",fd);
#endif
//    char temp[100];
//    read(11, temp, 100);
//    printf("Contents: %s\n",temp); 

/*  uint64_t syscall = 39, ret;
  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("int $0x80;");
  __asm__ volatile ("movq %%rax, %0;":"=r"(ret));
   return ret; */
    chdir("bin");
    while(1);
    return 0;
}
