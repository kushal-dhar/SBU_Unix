#include <libc.h>
#include <stdio.h>
#include <string.h>


char* fgets(char *dst, int max, long fp) {
 read_sys(0, fp, (long)dst, max);
  return dst;
}

void printf(char s[],char  str[]){
    int i =0;
    int j =0;
    int k =0;
    char p [100];
    while(s[i] != '\0'){
        if (s[i] == '%'){
            i+=2;
            while(str[j] != '\0'){
                p[k] = str[j];
                k++;
                j++;
            }
        }
        p[k] = s[i];
        k++;
        i++;
   }
    p[k] = '\0';
    k = strlen(p);
    write_sys(1,1,(long)&p,k);
}

void printInt(int i ){
    char str [500];
    char strCopy[500];
    int rem = 0;
    int k =0;
    char remChar;
    int count =0;
    while(i > 0){
    rem = i % 10;
    remChar = rem + '0';
    str[count] = remChar;
    i = i/10;
    count++;
    }
    str[count] = '\0';
    for(int j =count -1 ;  j >= 0; j--)
    {
        strCopy[k] = str[j];
        k++;
    }
    strCopy[k] = '\0';
    write_sys(1,1,(long)&strCopy,k);
}
