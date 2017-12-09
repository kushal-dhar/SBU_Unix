#include <stdio.h>
#include <sys/defs.h>
#include <unistd.h>
#include <cd.h>
#include <string.h>
#include <binaryoper.h>
#include <scripts.h>
#include <stdlib.h>
#include "exec_sh.h"
#include  <get_cmd.h>
#include <libc.h>
int main(int argc, char *argv[], char *envp[]) {
char path[100] = "rootfs/bin/\0\0\0";
char *str = (char *)mallocc(50); 
char temp[100];// = (char *) mallocc(100);
while(1){
   
//          str[0] = '\0';
 //         temp[0] ='\0';
          cwd();
          scan("%s",(void *)str);
          printf("string here: %s\n",str);
          int a = getpid();
	  printf("pid is: %d\n",a);
	  while(1);
         // check the currenht ditectory 
         if (strcmp(str, "pwd\0\0\0") ==0){
            cwd();
        }
        // check the change dir command cd
        else if( *(str) == 'c' && *(str+1) == 'd' && *(str+2) == ' '){
             for(int i =0; i <100;i++)   *(temp+i) = '\0';
             strcpy(temp,str+3);
             chdir(temp);
        }
        // check the cat command
        else if (*str == 'c' && *(str+1) == 'a' && *(str+2) == 't' && *(str+3) == ' '){
           int perm = 1; 
           for(int i =0; i <100;i++)   *(temp+i) = '\0';
           strcpy(temp,str+4);
           catt(temp,perm );
        }
        // check echo $PATH
        else if (strcmp(str, "echo $PATH\0\0\0")== 0){
               printf("\n%s",path);
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
                  printf("\n%s",temp);
        }else{
             printf("\n%s",":sbush : --command not found");
        } 

} 
#if 0
       getcwd(cwd,100);
       printf("%s", cwd);
       printf("%s","<sbush>");	
       fgets(cmd,100,1);
       if(cmd[0] == 'c' && cmd [1] == 'd'){
//        change_dir(cmd);
//       } else if((strstr(cmd, "export") != NULL) && (strlen(cmd) > 4)) {
//        set_cmd(cmd);

       /* if condition for path and PS1 is required */
       /* if condition for bckground */
        
        }  else if (strstr(cmd, "|") != NULL) {
   	   	pipe_proc(cmd);
     		getcwd(cwd,1024);
     		//printf("%s:sbush> ",cwd);
/*      } else if((strstr(cmd, "echo") != NULL) && (strstr(cmd,"$") != NULL)) {
        get_cmd(cmd+4);
        getcwd(cwd, 1024); */

        // Write script case here
        /*
	if(scriptCalled(cmd) == 1){
        	verifyAndRunScript(cmd,delim);
        }
        checkForBinary(cmd, ' ');             	
        */ 
	} else {
     	exec_shell(cmd);
        getcwd(cwd,1024);
      	printf("%s:sbush> ",cwd);
        } 
#endif
return 0;
}
