#include <stdio.h>
#include <sys/defs.h>
#include <unistd.h>
#include <string.h>
#include <libc.h>

char str[100];
int main(int argc, char *argv[], char *envp[]) {
/*char ** str = (char **)mallocc(sizeof(char *) * 10);
str[0] = (char*) mallocc(sizeof(char)*50);
strcpy(str[0],"hello\0");
printf("%s",str[0]);
while(1);
*/
//char path[100] = "rootfs/bin/\0\0\0";
//char *str =  (char *) mallocc(100);
    char temp[100];
    clear();
    while(1){
   
        str[0] = '\0';
        temp[0] ='\0';
        getcwd();
        scanf("%s",(void *)str);
        int slen = strlen(str);
        removespace(str); 
	if (slen == 0) {
	    continue;
	}
        // check the current ditectory 
        if (strcmp(str, "pwd\0\0\0") ==0){
            cwd2(temp);
            printf("%s\n",temp);
        }
        // Export
        else if (*str == 'e' && *(str+1) == 'x' && *(str+2) == 'p' ){
                if (  *(str+3) == 'o' &&  *(str+4) == 'r' && *(str+5) == 't' && *(str+6) == ' '){
                strcpy(str,str+7);
                setEchoValue(str);
                }
	}
         // check the change dir command cd
        else if( *(str) == 'c' && *(str+1) == 'd' && *(str+2) == ' '){
             for(int i =0; i <100;i++)   *(temp+i) = '\0';
             strcpy(temp,str+3);
              chdir(temp);
         }
        // check for sleep
        else if (*str =='s' && *(str+1) == 'l' && *(str+2) == 'e' && *(str+3) =='e' &&
		 *(str+4) == 'p'&& *(str+5) == ' '){
             uint64_t pid = fork();
             if(pid != 0){
                wait_pid(pid);
             }else{
                   execvpe("bin/sleep\0\0",str+6);
             }

        }
       // check for ls
        else if (*str == 'l' && *(str+1) == 's'){
               cwd2(temp);
               uint64_t pid = fork();
               if(pid != 0){
                   wait_pid(pid);
               }
               else{
                   char *tp = str;
                    if (*(str+2) == ' '){
                        tp=tp+3;
                  	addParent(temp,tp);
 			strcpy(str,tp); 
                        execvpe("bin/ls\0\0",str);
                    } else {
                         tp=tp+2;
                         addParent(temp,tp);
                         strcpy(str,tp);
                         if (strlen(str) == 0){
                               str[0]= '\0';
                         }
                         execvpe("bin/ls\0\0",str);
                    }
              }
        } 
        // Check for ps
        else if (*str=='p' && *(str+1) == 's'){
               uint64_t pid = fork();
               if(pid != 0){
                   wait_pid(pid);
               }
               else{
                    execvpe("bin/ps\0\0",str);
              } 
       }
	// Check for kill -9
        else if (*str=='k' && *(str+1) == 'i' && *(str+2) == 'l' && *(str+3) == 'l' && *(str+5) == '-' && *(str+6) == '9' ){
               uint64_t pid = fork();
               if(pid != 0){
                   wait_pid(pid);
               }
               else{
                    execvpe("bin/kill\0\0",str+7);
              }
        }

        // Check for sbush
  	else if (*str == '.' && *(str+1) == '/' && *(str+2) == 's'
		 && *(str+3) == 'b' && *(str+4) == 'u' 
		&& *(str+5) == 's' && *(str+6) == 'h') {
               int len = strlen(str);
               uint64_t pid = fork();
               if(pid != 0){
                   wait_pid(pid);
               }
               else{
                    if(*(str+2) == ' ' && len > 2){
                           addDelimiter(str+7);
                    }
                    execvpe("bin/sbush\0\0",str+7);
              }
	}
        // Check for sbang
        else if (*str == '.' &&  *(str+1) == '/' && 
 		*(str+slen-3) == '.'  &&  *(str+slen-2) == 's'
		 &&  *(str+slen-1) == 'h') {
                 char *tp = str;
                 cwd2(temp);
                 tp= tp+ 2;
                 addParent(temp,tp);
 		 strcpy(str,tp);
                 executeSbang(str);
        }
          // Check for ECHO
        else if (*str == 'e' && *(str+1) == 'c' && *(str+2) == 'h'
                 && *(str+3) == 'o' && *(str+4) == ' ')
              {
               uint64_t pid = fork();
               if(pid != 0){
                   wait_pid(pid);
               }
               else{

                    execvpe("bin/echo\0\0",str+5);
              }
        }
         // check for Cat
        else if (*str == 'c' && *(str+1) == 'a' && *(str+2) == 't') {
             cwd2(temp);
             uint64_t pid = fork();
             if(pid != 0){
                wait_pid(pid);
             }else{
                   char *tp = str;
                   tp = tp + 4;
                   addParent(temp,tp);
                   strcpy(str,tp);
                   execvpe("bin/cat\0\0",str);
             }
        }
	// Incase of exit command, just exit from here
        else if (*str == 'e' && *(str+1) == 'x' && *(str+2) == 'i' && *(str+3) == 't' ) {
	    exit();
	}
        // clear screen 
        else if (strcmp(str,"clear\0\0") == 0){
          clear();
        }
        else{
             printf("-sbush : %s: command not found\n", str);
        }

}
return 0;
}
