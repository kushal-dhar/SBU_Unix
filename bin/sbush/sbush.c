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
int main(int argc, char *argv[], char *envp[]) {
char cwd[1024];
char cmd[1024];
//char delim = ' ';
while(1){
       getcwd(cwd,1024);
       printf ("%s", cwd);
       printf("%s","<sbush>");	
       fgets(cmd,100,1);
       if(cmd[0] == 'c' && cmd [1] == 'd'){
        change_dir(cmd);
//       } else if((strstr(cmd, "export") != NULL) && (strlen(cmd) > 4)) {
//        set_cmd(cmd);

       /* if condition for path and PS1 is required */
       /* if condition for bckground */
        
        }  else if (strstr(cmd, "|") != NULL) {
   	   	pipe_proc(cmd);
     		getcwd(cwd,1024);
     		printf("%s:sbush> ",cwd);
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
}
return 0;
}
