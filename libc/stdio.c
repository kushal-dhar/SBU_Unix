#include <libc.h>
#include <stdio.h>
#include <string.h>

#define va_start(v,l)  __builtin_va_start(v,l)
#define va_arg(v,l)    __builtin_va_arg(v,l)
#define va_end(v)      __builtin_va_end(v)

typedef  __builtin_va_list va_list;

char          result[1000];

char* fgets(char *dst, int max, long fp) {
 read_sys(0, fp, (long)dst, max);
  return dst;
}

void  convertPointerAddress2String(unsigned long long n, char str[]){
    char *string2 = NULL;
    int i =0;
    while (n >0){
        int rem1 = n %16;
        long div = n/16;
        if(rem1 >= 0 && rem1 <= 9){
            string2[i] = (char) (rem1  + '0');
        }else{
            rem1 -= 10;
            string2[i] = (char) (rem1 + 'A');
        }
        n = div;
        i++;
    }
    i--;
    int j =2;
    str[0] = '0';
    str[1] = 'x';
    while(i >= 0  ){
        str[j++] = string2[i--];
    }
    str[j] = '\0';
    i = 0;
    while(str[i] != '\0') {
        i++;
    }
    if (i == 2) {
        str[i++] = '0';
        str[i] = '\0';
    }
    return;
}

void  convertHex2String(unsigned long n, char str[]){
    char * string = NULL;
    int i = 0;
    int j = 0;

    while (n >0){
        int rem1 = n %16;
        int div = n/16;
        if(rem1 >= 0 && rem1 <= 9){
            string[i] = (char) (rem1  + '0');
        }else{
            rem1 -= 10;
            string[i] = (char) (rem1 + 'A');
        }
        n = div;
        i++;
    }
    i--;
    j = 2;
    str[0] = '0';
    str[1] = 'x';
    while(i >= 0  ){
        str[j++] = string[i--];
    }
    str[j] = '\0';
    return;
}

void strtoint (int num, char str[])
{
    int i = 0;

    while (str[i] != '\0') {
        str[i] = '\0';
        i++;
    }
    i = 0;

    if (num > 9)
    {
        i = 0;
        strtoint (num / 10, str);
        while (str[i] != '\0')
        {
            i++;
        }
        str[i++] = (int)(num % 10) + '0';
        str[i] = '\0';
    }
    else if(num == 0) {
        str[0] = '0';
        str[1] = '\0';
    }
    else
    {
        i = 0;
        while (str[i] != '\0')
        {
            i++;
        }
        str[i++] = (int)num + '0';
        str[i] = '\0';
    }
    return;
}


void printf(char *string, ...) {
    va_list       list;
    va_start(list, string);
    char          char_val;
    int           int_val   = 0;
    char         *str_val   = NULL;
    char          str[100];
    int           i         = 0;
    unsigned long long_val  = 0;
    int           count     = 0;


/*    while (i < 500) {
	result[i] = '\0';
	i++;
    }*/
    i = 0;
    while(result[i] != '\0') {
	result[i++] = '\0';
    }

    while( *string != '\0')
    {
        while (*string != '%' && *string != '\0') {
            if (*string == '\n') {
		result[count++] = '\n';
		string ++;
            } else {
		result[count++] = *string++;
            }
        }
        if (*string == '%') {
            string++;
            switch(*string) {
                case 'c':
                    char_val = va_arg(list, int);
		    result[count++] = char_val;
		    string++;
                    break;
                case 'p' :
                    long_val = (unsigned long)va_arg(list, unsigned long);
                    convertPointerAddress2String(long_val, str);
                    i = 0;
                    while(str[i] != '\0') {
			result[count++] = str[i++];
                    }
                    break;
                case 'd':
                    int_val = va_arg(list, int);
                    i = 0;
                    while(str[i] != '\0') {
                        str[i++] = '\0';
                    }
                    i = 0;
                    strtoint(int_val, str);
                    while(str[i] != '\0') {
			result[count++] = str[i++];
                    } 
                    break;
                case 's':
                    str_val = (char *)va_arg(list, char *);
                    while (*str_val != '\0') {
			result[count++] = *str_val;
			str_val++;
                    }
                    break;
                case 'x' :
                    int_val = va_arg(list, int);
                    i = 0;
                    convertHex2String(int_val, str);
                    while(str[i] != '\0') {
			result[count++] = str[i++];
                    }
                    break;
                default:
                    break;
            }
            string++;
        }
    }
    result[count] = '\0';
    va_end(list);

    i = 0;
	
    i = syscall_2((uint64_t)1, (uint64_t)result);
    if (i != 0) {
	return;
    }
}

void scanf(char *str, void *buf) {
    char        temp_buf[100];
    uint64_t    syscall         = 4;
    uint64_t    i               = 0;
    char       *ptr =(char *) buf;
    while(*ptr != '\0') {
   	*ptr = '\0';
	ptr++;
    }
    while(*str != '\0') {
        temp_buf[i] = *str++;
        i++;
    }
    temp_buf[i] = '\0';

    i = syscall_2(syscall, (uint64_t)buf);
    if (i != 0) {
 	printf("Error in reading from stdin\n");
	return;
    }

    if(strcmp(temp_buf, "%s") == 0) {
/*        int i = 0;
        while (temp_buf[i] != '\0') {
            *(char *)buf = temp_buf[i++];
            (char *)buf++;
        }
        *(char *)buf = '\0'; */
    }
    else if (strcmp(temp_buf, "%d") == 0) {
	int val = 0;
        strtoint(val, buf);
	*(int *)buf = val;
    }
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

void clear() {
    uint64_t syscall = 91;

    syscall_1((uint64_t)syscall);
    return;
}
