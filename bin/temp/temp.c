#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libc.h>

int main(int argc, char *argv[], char *envp[]) {
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
    printf("Argv: %s\n",str[1]);
    cwd();
    int b = getpid();
    printf("PID in execve %d\n",b);
    printf("Even printf is returning properly\n");
    while(1);
    return 0;
}
