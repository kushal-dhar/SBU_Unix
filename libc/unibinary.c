#include <unistd.h>
#include <string.h>
void checkingForBinary(char *s){
        char str[100];
        char temp[100];
       str[0]='\0';
       temp[0]= '\0';
       strcpy(str,s);
	removespace(s);
        // sleep case
	if (*str =='s' && *(str+1) == 'l' && *(str+2) == 'e' && *(str+3) =='e' &&
                 *(str+4) == 'p'&& *(str+5) == ' '){
         uint64_t pid = fork();
        	 if(pid != 0){
               		wait_pid(pid);
        	 }else{
               		execve("bin/sleep\0\0",str+6);
        	 }	
        }	
  	// check for Cat
        else if (*str == 'c' && *(str+1) == 'a' && *(str+2) == 't' && *(str+3) == ' ') {
            cwd2(temp);
             uint64_t pid = fork();
             if(pid != 0){
                wait_pid(pid);
             }else{
                   char *tp = str;
                   tp = tp + 4;
                   addParent(temp,tp);
                   strcpy(str,tp);
                   execve("bin/cat\0\0",str);
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
                    if (*(str+3) == ' '){
                        tp=tp+3;
                        addParent(temp,tp);
                        strcpy(str,tp);
                        execve("bin/ls\0\0",str);
                    } else {
                         tp=tp+2;
                         addParent(temp,tp);
                         strcpy(str,tp);
                         if (strlen(str) == 0){
                               str[0]= '\0';
                         }
                         execve("bin/ls\0\0",str);
                    }
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
}
