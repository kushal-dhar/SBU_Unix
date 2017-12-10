#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libc.h>

int main(int argc, char *argv[], char *envp[]) {
    printf("Argv: %s\n",argv[0]);
    cwd();
    int b = getpid();
    printf("PID in execve %d\n",b);
    printf("Even printf is returning properly\n");
    while(1);
    return 0;
}
