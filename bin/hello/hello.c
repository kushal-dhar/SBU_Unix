#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
    uint64_t a = getpid();
    while(a);
    return 0;
}
