#include <stdio.h>
#include <sys/defs.h>
#include <unistd.h>
#include <cd.h>
#include <string.h>
#include <scripts.h>
#include <get_cmd.h>
#include <libc.h>

char str[100];
int main(int argc, char *argv[], char *envp[]) {
/*char ** str = (char **)mallocc(sizeof(char *) * 10);
str[0] = (char*) mallocc(sizeof(char)*50);
strcpy(str[0],"hello\0");
printf("%s",str[0]);
while(1);
*/
char path[100] = "rootfs/bin/\0\0\0";
//char *str =  (char *) mallocc(100);
char temp[100];
clear();
while(1){
   
          str[0] = '\0';
          temp[0] ='\0';
          cwd();
          scan("%s",(void *)str);
         // check the current ditectory 
         if (strcmp(str, "pwd\0\0\0") ==0){
            cwd2(temp);
            printf("%s",temp);
        }
        // check the change dir command cd
        else if( *(str) == 'c' && *(str+1) == 'd' && *(str+2) == ' '){
//             for(int i =0; i <100;i++)   *(temp+i) = '\0';
//             strcpy(temp,str+3);
             uint64_t pid = fork();
             if(pid != 0){
                wait_pid(pid);
             }else{
//                   addDelimiter(str+4);
                   execve("bin/cd\0\0",str+3);
             }
        }
        // check echo $PATH
        else if (strcmp(str, "echo $PATH\0\0\0")== 0){
               printf("%s",path);
        }
        else if (*str =='e' && *(str+1) == 'c' && *(str+2) == 'h' && *(str+3) =='o' && *(str+4) == ' '){
                  int len = strlen(str);
                  for(int i =0; i <100;i++)   *(temp+i) = '\0';
                  if(*(str+5) == '"'){
                    strcpy(temp,str+6);
                    temp[len-2] = '\0';
                  }else {
                      strcpy(temp,str+5);
                  }
                  printf("%s",temp);
        }
        // check for sleep
//	else if (strstr(str, "sleep") != 0) {
        else if (*str =='s' && *(str+1) == 'l' && *(str+2) == 'e' && *(str+3) =='e' &&
		 *(str+4) == 'p'&& *(str+5) == ' '){
             uint64_t pid = fork();
             if(pid != 0){
                wait_pid(pid);
             }else{
                   execve("bin/sleep\0\0",str+6);
             }

        }
        // check for Cat
//        else if (strstr(str, "sleeping") != 0) {
        else if (*str == 'c' && *(str+1) == 'a' && *(str+2) == 't' && *(str+3) == ' ') {
             uint64_t pid = fork();
             if(pid != 0){
                wait_pid(pid);
             }else{
//                   addDelimiter(str+4);  
                   execve("bin/cat\0\0",str+4);
             }
        }
        // check for ls
        else if (*str == 'l' && *(str+1) == 's'){
               int len = strlen(str);
               uint64_t pid = fork();
               if(pid != 0){
                   wait_pid(pid);
               }
               else{
                    if(*(str+2) == ' ' && len > 2){
    			   addDelimiter(str+3);
                    } 
                    execve("bin/ls\0\0",str+3);
              }
        } 
        // Check for ps
        else if (*str=='p' && *(str+1) == 's'){
//              printAllProcess();
//               int len = strlen(str);
               uint64_t pid = fork();
               if(pid != 0){
                   wait_pid(pid);
               }
               else{
/*                    if(*(str+2) == ' ' && len > 2){
                           addDelimiter(str+3);
                    } */
                    execve("bin/ps\0\0",str);
              }
        }
        // Check for sbush
  	else if (*str == '.' && *(str+1) == '/' && *(str+2) == 's' && *(str+3) == 'b' && *(str+4) == 'u' && *(str+5) == 's' && *(str+6) == 'h') {
               int len = strlen(str);
               uint64_t pid = fork();
               if(pid != 0){
                   wait_pid(pid);
               }
               else{
                    if(*(str+2) == ' ' && len > 2){
                           addDelimiter(str+7);
                    }
                    execve("bin/sbush\0\0",str+7);
              }
	}
	// Incase of exit command, just exit from here
        else if (*str == 'e' && *(str+1) == 'x' && *(str+2) == 'i' && *(str+3) == 't' ) {
	    exit();
	}
        else{
             printf("%s",":sbush : --command not found");
        }

}
return 0;
}
