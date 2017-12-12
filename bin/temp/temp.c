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
//    cwd();
    char *str = (char *)mallocc(sizeof(char)*4000);
    strcpy(str, "Hello world, how you doing!!\n");
    printf("str: %s",str);
    char *str1 = (char *)mallocc(sizeof(char)*400000);
    strcpy(str1, "This is Joey!!\n");
    printf("str: %s",str1);
    printf("Argc here:%d\n ",argc);
    for (int i = 0; i < argc; i++) {
	printf("Arg: %s\n",argv[i]);
    }
//    exit();
    while(1);
    return 0;
}
