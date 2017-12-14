#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/defs.h>


int main(int argc, char *argv[], char *envp[]) {
    uint64_t pid = fork();
    char str[10] = "    ";

    if(pid != 0){
        wait_pid(pid);
    }
    else{
         execve("bin/sbush\0\0",str);
    }
    return 0;
}
