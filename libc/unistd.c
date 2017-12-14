#include <libc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <unibinary.h>
#define ROOT 'rootfs/'
extern uint64_t global_val;
extern int temp_val;
char* getcwd(char * str, long size){
long syscall = 79;
long ret ;
__asm__ volatile (
    "mov %1, %%rax\n\t"
    "mov %2, %%rdi\n\t"
    "mov %3, %%rsi\n\t"
    "syscall\n\t"
    "mov %%rax, %0\n\t"
    : "=r"(ret)
    : "a"(syscall), "b"(str), "c" (size)
    : "rdi", "rsi"

); 
return(char *) ret;
}

int waitpid(int pid, int *wstatus, int options) {

   /* System call for wait is 61*/
   pid = syscall_wait(61, pid, wstatus, options);
   return pid;
}

int execvp(char *file, char *argv[]) {
   int ret;

   /* System call for execve is 59 */
   ret = syscall_execve(59, file, argv);
   return ret;
}


int pipe(int fd[]) {
  int ret;

  /* System call for pipe is 22 */
  ret = syscall_pipe(22,(long*)fd);
  return ret;
}

int close(int fd) {
  int ret;

  /* 3 is the syscall # for close */
  ret = syscall_close(3, fd);
  return ret;
}

int dup2(int oldfd, int newfd) {
   int fd;

   /* System call for dup2 is 33 */
   fd = syscall_dup(33, (long)oldfd, (long)newfd);
   return fd;
}
/*
int get_pid() {
    uint64_t   syscall = 39;
    uint64_t   pid    = 0;
    
    pid = syscall_2(syscall, pid);
    return pid;
}
*/
uint64_t open_dir(char *filename) {
    char temp_buf[100];
    int      i       = 0;
    uint64_t ret_val = 0;

    while (*filename != '\0') {
	temp_buf[i] = *filename++;
	i++;
    }
    temp_buf[i] = '\0';

    i = 89;    
    //Syscall number for open_dir
    ret_val = syscall_2((uint64_t)i, (uint64_t)temp_buf); 
    if(ret_val== 999){
     printf("\n%s","-sbush: cd:  No such file or directory");
    }

    return ret_val;
}

void read_dir(uint64_t fd) {
    int syscall = 90;
    int ret     = 0;

    ret = syscall_2((uint64_t)syscall, (uint64_t)fd);
    if (ret != 0) {
	printf("Error in reading directory");
    }
    return;
}

int open(char *filename, int perm) {
    int           syscall = 0;
    uint64_t      fd      = 0;
    char          temp_buf[100];
    int           i       = 0;

    while (*filename != '\0') {
        temp_buf[i] = *filename++;
        i++;
    }
    temp_buf[i] = '\0';

    fd = syscall_3((uint64_t)syscall, (uint64_t)temp_buf, (uint64_t)perm);

    return (int)fd;
}

void read(uint64_t fd, char *buf, int size) {
    int syscall = 2;
    int ret;

    ret = syscall_2((uint64_t)50, (uint64_t)size);
    ret = syscall_3((uint64_t)syscall, (uint64_t)fd, (uint64_t)buf);
    if (ret != 0) {
        printf("Error in reading file");
    }
    return;
}

void chdir(char * dir) {
   uint64_t r =0;
    /* System call for pipe is 80 */
   r =  syscall_2(80,(uint64_t) dir);
   if(r != 0){
       printf("Something is not right");
   }
}

void  cwd() {
  /* System call for pipe is 79 */
  uint64_t  ret =0;

  ret =  syscall_1(79);
  if(ret != 0){
      printf("%s","Error");
  }
  return;
}

void* mallocc(int size){
  uint64_t  ch;
  ch = syscall_2(9,(uint64_t) size);
  return (void *)ch;
}


uint64_t getpid(){
 uint64_t pid=0;
  pid = syscall_pid(39);
 return pid;
}

int fork() {
    int syscall = 57;
    int ret_val = 0;

    ret_val = syscall_1((uint64_t)syscall);
    return ret_val;
}

void wait_pid(uint64_t pid) {
    int syscall = 247;
    int ret     = 0;

    ret = syscall_2((uint64_t)syscall, (uint64_t)pid);
    if (ret != 0) {
	printf("Error in exiting. You are going to exit too\n");
    }
    return;
}

void execve(char *filename, char *ex_argv) {
    int syscall = 59;
    int ret = 0;

    ret = syscall_3((uint64_t)syscall, (uint64_t)filename, (uint64_t)ex_argv);
    if (ret != 0) {
	printf("Error in executing binary\n");
    }
    return;
}
void  cwd2(char * buf ) {
  /* System call for pipe is 79 */
  uint64_t ret =0;  
  ret =  syscall_2(78,(uint64_t) buf);
  if(ret != 0){
    printf("\nSomething fishy");
  }
}


void exit() {
    int syscall = 60;
    int ret     = 0;

    ret = syscall_1((uint64_t)syscall);
    if (ret != 0) {
	printf("Error in exiting \n");
    }
    return;
}
void executeSbang(char * str){
 uint64_t  fd = open(str, 0);
 str[0] = '\0';
 read(fd,str, 1000) ;
 if ( str[0] == '#' && str[1] == '!'){
      char *temp = str;
      while(*temp != '\n') temp++;
      temp++;
      str = temp;
      while(*temp != '\0'){
    	char *temp2 = temp;
        while(*temp != '\n') temp++;
        *temp = '\0';
        temp++;
        checkingForBinary(temp2);  
      }
      
 }
 else{
	printf("-sbush:  No such file or directory");
 }
}

void addParent(char *parent, char *str){
char temp[100];
strcpy(temp,parent+7);
strcat(temp,str);
strcpy(str,temp);
int len = strlen(str);
if ( *(str+len-1) != '/' &&  *(str+len-3) != '.' && *(str+len-2) != 's' && *(str+len-1) != 'h' && 
      *(str+len-4) != '.' && *(str+len-3) != 't' && *(str+len-2) != 'x' && *(str+len-1) != 't'){
     *(str+len)= '/';
     *(str+len+1) = '\0';
} 

}

void kill(uint64_t pid) {
    int syscall = 62;
    int ret     = 0;

    ret = syscall_2((uint64_t)syscall, (uint64_t)pid);
    if (ret != 0) {
	printf("Process Kill unsuccessful\n");
    }

    return;
}

void setEchoValue(char * s){
    int flag =0;
   // Number of quotes (")
   int qcount = 0;
   // Number of equal to sign (=)
   int eqcount =0;
   char * temp = s;
   while(*temp != '\0'){
        if (*temp == '='){ 
           eqcount++;
          
        }
        if (*temp =='\"' || *temp == '\''){
           qcount++;
           flag =1;
        }
       if (flag ==1){
          *temp = *(temp+1);
       }
         temp++;
   }
   
   if (eqcount == 1 && qcount == 2)
   {   int len = strlen(s);
       s[len-1] ='\0';
       uint64_t ret =  syscall_2(5,(uint64_t) s);
       if (ret == 999)
           printf("Some issue with Echo");

   }
   else
       printf("-sbush: wrong export statement");
}

void printEchoVar(char * s){
  uint64_t ret =  syscall_2(6,(uint64_t) s);
  if (ret == 999){
      printf("Some issue with Echo");
   }
}
   
