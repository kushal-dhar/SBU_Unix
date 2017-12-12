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
    clear();
/*    char str[100];
    scan("%s",(void *)str);
    print("%s\n",str);
    scan("%S",(void *)str);
    print("%s\n",str); */
    
    int fd = open_dir("bin/");
    if (fd == -1) {
	printf("Not a valid directory\n");
    }
    printf("Working %d\n",fd);
    read_dir(fd);

    fd = open("random.txt",3);
    if (fd == -1) {	
	printf("Not a valid file\n");
    }
    printf("File number: %d\n",fd);

    char temp[1000];
    read(fd, temp, 5);
    printf("Contents: %s\n",temp); 
    read(fd, temp, 5);
    printf("Contents: %s\n",temp);

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
//     int a = getpid();
//     printf("PID: %d\n",a);

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
    int a = test(10);
    printf("Value: %d\n",a);

//    int b = getpid();
//    printf("PID again %d\n",b);
    int pid = fork();
    if (pid != 0) {
        //cwd();
        printf("Inside parent\n");
	wait_pid(pid);
	printf("I came bacl here \n");
	while(1);
    }

    else {
	char *str = (char *)mallocc(sizeof(char) * 100);
	strcpy(str, "Hello");
	strcat(str, "\n");
	strcat(str, "World\n");
        strcat(str, "Again\n\0");
        printf("Printf is also working %s    \n",str);
        execve("bin/test", (char *)str);
    }
/*    char s[100];
    scan("%s",s);
    printf("string: %s",s); */
//    cwd(); 
    printf("Going to exit now\n");
    exit();
//    while(1);
    return 0;
}
