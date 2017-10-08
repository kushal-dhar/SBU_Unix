#include <string.h>
#include <binaryoper.h>
#include <stdio.h>
#include <cd.h>
#include <unistd.h>
#include <stdio.h>
void  verifyAndRunScript(char path[],char delim){
 int file ;
// int count;
 char *temp = NULL;
// count = 0;
 int len ;
printf("%s","I am here");
if((file = open(path,3)) == 0){
      printf("%s","file path is not found");
 }
printf("%s","I am here too\n:");
 printInt(file);
 temp = fgets(temp,1000,file);
    char *line[100];
    printf("%s",temp);
    len = strlen(temp);
    temp[len - 1] = '\0';
    split_string(temp, '\n', line);
/*    while(i < 0; temp[i][0] != '\0'; i++) {
    if ((count !=0) && (temp[0] != ' ') && (temp[0] != '\0') &&
      (temp[0] != '\r') && (temp[0] != '\n')){
       printf("Line read in file : %s \n",temp);
       if(temp[0] == 'c' && temp[1] == 'd'){*
                 change_dir(temp);
       }else{
       checkForBinary(temp,delim);
        }
     }
     count++;*/
}

int scriptCalled(char str[]){
int len = strlen(str);
printf("%s\n","len");
printInt(len);
char delim = ' ';
int i ,k;
len-=1;

if(len > 6){
            if(strstr(str,".sbush") != 0){
                if(strstr(str,"chmod") == 0){
		printf("%s","lovely");
		char  ch[100] = {'c','h','m','o','d',' ','a','+','x',' '};
                k = 0;
                i =10;
                printf("%s","Enter for");
                for( ; i < 10+len ; i++, k++){
                	ch[i] = str[k];
                }
                ch [i] = '\0';
                checkForBinary(ch,delim);
           }
        return 1;
        }
}
return 0;
}

