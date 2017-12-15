#include <stdio.h>
#include <libc.h>
#include <string.h>



int main(int argc, char *argv[], char *envp[]) {
    uint64_t syscall = 270;
    uint64_t ret_val = syscall_1((uint64_t)syscall);
    if (ret_val != 0){
        printf("Something is fishy\n");
   }

    return 0;
}
