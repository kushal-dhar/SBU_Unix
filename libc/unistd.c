#include <libc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define ROOT 'rootfs/'
extern uint64_t global_val;

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

/*
int chdir ( char * str){
long syscall = 80;
long  ret ;
__asm__ volatile (
    "syscall\n\t"
    "mov %%rax, %0\n\t"
    : "=a"(ret)
    : "r"(syscall), "r"(str)
    : "rdi"
);
return  (int)ret;
}
*/
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

pid_t fork() {
  pid_t ret = 0;

  /* System call for fork is 57 */
  ret = syscall_fork(57, ret);
  return ret;
}

#if 0
int open(const char *path, int flag) {
  int ret;

  /* 2 is the system call for open */
  ret = syscall_open(2, (long)path, flag);
  return ret;
}
#endif

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

int get_pid() {
    uint64_t   syscall = 39;
    uint64_t   pid    = 0;
    
    pid = syscall_2(syscall, pid);
    return pid;
}

int open_dir(char *filename) {
    char temp_buf[100];
    int      i       = 0;
    uint64_t ret_val = 0;

    while (*filename != '\0') {
	temp_buf[i] = *filename++;
	i++;
    }
    temp_buf[i] = '\0';

    i = 89;    //Syscall number for open_dir
    ret_val = syscall_2((uint64_t)i, (uint64_t)temp_buf); 

    return ret_val;
}

void read_dir(int fd) {
    int syscall = 90;

    syscall_2((uint64_t)syscall, (uint64_t)fd);
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

    fd = syscalls_3((uint64_t)syscall, (uint64_t)temp_buf, (uint64_t)perm);
    if (fd > 10000) {
        fd = global_val;
    }
    return (int)fd;
}

void read(int fd, char *buf, int size) {
    int syscall = 2;

    syscall_4((uint64_t)syscall, (uint64_t)fd, (uint64_t)buf, (uint64_t)size);
    return;
}
int chdir(char * dir) {
  int ret;
  char dirname[100];
  char curr_dir[100];
  char temp[100];
  int len = strlen(dir);
  if(len == 0){
  return 0;
  }
  char * str =  cwd(curr_dir);
  int len2 = strlen(str);
  for (ret = 0; ret < 100; ret++) 
 {   dirname[ret] = '\0';  temp[ret] = '\0';}
  ret = 0;
  strcpy(dirname, str+7);
  dirname[len2-7] = '\0';
  if(len >=1 && *(dir+0) == '/'){
      // case cd /
      temp[0]='\0';
      if(len > 1){
        // case cd  /bin
        strcpy(temp,dir);
      }
      
  }else if (len >=2 && *(dir+0) =='.' && *(dir+1) =='.'){
     // go to parent case cd ..
      strcpy(temp,dirname);
      int rr = substr_tillchar(temp, '/');
      if (rr == -1 ||( strcmp(str,"rootfs/")==0)){
          for (int i =0; i <100; i++){  temp[i]='\0';}
     }
     if  (len >= 4){
        // go to the directory from parent : case cd ../bin  
       strcat (temp,dir+3);
       strcat(temp,"*");   
   }
   else{
      len = strlen(temp);
      if(temp[len-1] == '/'){
         temp[len-1] = '\0';
       }
      strcat(temp,"*");
   }
  }else if ((len >=2 && *dir == '.' && *(dir+1) =='/') || (len > 0)){
        // go to the directory from current : case cd ./bin or cd bin
        strcpy(temp,dirname);
        if (len >=2 && *dir == '.' && *(dir+1) =='/') {
            strcat(temp,dir+2);
        }
        else {
            strcat(temp,dir);
        }
 }
 len = strlen(temp);
if(len == 0){
  temp[0] = '/';
  temp[1] = '\0';
 }else if (len >= 1) {
   if (temp[len-1] != '/'){
       strcat(temp,"/");
   }
 }
 for(int i =0; i <100;i++) dirname[i]='\0';
 strcpy(dirname,temp);
  /* System call for pipe is 80 */
 ret = syscall_2(80,(uint64_t)dirname);
 if (ret < 100) {
    ret = global_val;
 }
 if(ret == 999){
   char res[100];
   strcpy(res,"Wrong directory address");    
   printf("\n%s",res);
 }
  return 0;
}
char * cwd(char * cwddir) {
   char temp[100];
  /* System call for pipe is 79 */
   syscall_2(79,(uint64_t) temp);
   strcpy(cwddir, temp);
   return cwddir;
}

void*  mallocc(int size){
  uint64_t retAddr; 
  retAddr = syscall_2(9,(uint64_t) size);
  return (void *)retAddr;
}

