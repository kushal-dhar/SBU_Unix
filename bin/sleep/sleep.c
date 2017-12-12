#include <stdio.h>
#include <libc.h>
#include <string.h>



int main(int argc, char *argv[], char *envp[]) {
    uint64_t syscall   = 35;
    int      ret       = 0;
    uint64_t duration  = atoi(argv[0]);
    /* System call for nano sleep is 35 */
    ret = syscall_2(syscall,(uint64_t) duration);
    if (ret != 0) {
    	printf("Something is fishy!! Not able to sleep");
    }

    return 0;
}
