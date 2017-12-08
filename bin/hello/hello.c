#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libc.h>
#include <test.h>

int test(int x) {
    printf("I am inside test and going to return %d\n",x);
    return 10;
}
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

     char str[100];
     strcpy(str,"bin/\0");
     chdir(str);
     test(25);
     cwd();
     char t[100];
     strcpy(t,"extra/\0");
     chdir(t);
//     chdir(s);

/*    strcpy(s,"extra/");
     chdir(s);
    strcpy(s,"extra2/");
     chdir(s);
     cwd();*/
   /*  strcpy(s,"extra/");
     chdir(s);
     strcpy(s,"extra2/");
     chdir(s);
//     cwd(s);
    mallocc(10);
    char *ss = (char *)global_val; 
    ss[0] = 'a'; 
    ss[1] = '\0';
    printf("Addr is %s",ss);  
*/  
// uint64_t pid = getpid();
// printf("pid is %d",pid);
//  catt("random.txt", 2);
/* char *s = (char *)mallocc(10);
   s[0]='w';
   s[1]='\0';
   kprintf("\nkiller who %s",s);
*/ 
    printf("Hello world\n");
    printf("Hello world\n");
    printf("Hello world\n");
    printf("Hello world\n");
    printf("Hello world\n");
    printf("Hello world\n");
    int a = test(10);
    printf("Value: %d\n",a);
    test(20);
    a = getpid();
    printf("PID: %d\n",a);
/*    char s[100];
    scan("%s",s);
    printf("string: %s",s); */

    while(1);
    return 0;
}
