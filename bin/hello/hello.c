#include <stdio.h>
#include <unistd.h>
#include <string.h>
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
    int fd = open("bin/temp.txt",3);
    if (fd == -1) {	
	printf("Not a valid file\n");
    }
    printf("File number: %d\n",fd);

    char temp[1000];
    read(fd, temp, 1000);
    printf("Contents: %s\n",temp); 
#endif
/*    clear();
    char temp1[1000];
    scan("%s",temp1);
    printf("Read: %s\n",temp1);
    scan("%s",temp1);
    printf("Read: %s\n",temp1);
    scan("%s",temp1);
    printf("Read: %s\n",temp1);
*/

#if 0
    char str[1000];
    scan("%s",(void*)str);
    print("%s\n",str);
    scan("%s",str);
    print("%s\n",str);
    scan("%s",str);
    print("%s\n",str);
#endif

/*  uint64_t syscall = 39, ret;
  __asm__ volatile ("movq %0, %%rbx;"::"r"(syscall));
  __asm__ volatile ("int $0x80;");
  __asm__ volatile ("movq %%rax, %0;":"=r"(ret));
   return ret; */
   
//     char s[100];
//     strcpy(s,"kbin/");
//     chdir(s);
   uint64_t* s = (uint64_t*)mallocc(10); 
   printf("Addr is %p",s);  
   while(1);
    return 0;
}
