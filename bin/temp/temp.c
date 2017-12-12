#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libc.h>

int main(int argc, char *argv[], char *envp[]) {
#if 0
    char str[5][50];
    char temp_str[50];
    int i = 0;
    int j = 0;
    char *str1;
    while(j < argc) {
	str1 = (char *)argv+(j*64);
	i = 0;
        while (*str1 != '\0') {
	    temp_str[i++] = *(char *)str1;
	    str1++;
        }
        temp_str[i] = '\0';
        strcpy(str[j], temp_str);
	j++;
	i = 0;
	while(temp_str[i] != '\0') {
	    temp_str[i++] = '\0';
	}
    }
#endif
    cwd();
    int i = 0;
    int b = getpid();
    printf("PID in execve %d\n",b);
        int pid = fork();
        if (pid != 0) {
        //cwd();
	        printf("\nInside parent %d",b);
        	wait_pid(pid);
	        printf("    I came bacl here i   %d",b);
        	exit();
        }

        else {
//        int b = getpid();
//        printf("Child pid is: %d\n",b);
        	chdir("bin/");
	        cwd();
        	char *str = (char *)mallocc(sizeof(char) * 100);
	        strcpy(str, "Hello");
        	strcat(str, "\n");
	        strcat(str, "World\n");
        	strcat(str, "Again\n\0");
        	printf("     Printf is also working %d ",str,i);
	        execve("bin/temp", (char *)str);
         }
    return 0;
}
