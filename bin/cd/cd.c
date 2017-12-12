#include <stdio.h>
#include <unistd.h>
#include <libc.h>
#include <sys/defs.h>


void chdir(char * dir) {
   uint64_t ret =0;
    /* System call for pipe is 80 */
   ret =  syscall_2(80,(uint64_t) dir);
   if(ret != 0){
       printf("Something is not right");
   }
}


int main(int argc, char *argv[], char *envp[]) {
  chdir(argv[0]);
//  ls(argc,argv);
  return 0;
}
