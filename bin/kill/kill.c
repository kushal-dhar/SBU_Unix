#include <stdio.h>
#include <libc.h>
#include <string.h>



int main(int argc, char *argv[], char *envp[]) {
    int      syscall = 62;
    int      ret     = 0;
    uint64_t pid     = 0;

    pid = (uint64_t)atoi(argv[1]);
    ret = syscall_2((uint64_t)syscall, (uint64_t)pid);
    if (ret != 0) {
        printf("Process Kill unsuccessful\n");
    }

    return 0;
}
