#if 0
#include <sys/kprintf.h>
#include <stdarg.h>
#include <sys/defs.h>

volatile char *video     = ( char*)0xB8000;
int           colour    =  31;
void  atoi(int num) {
/*    char    *str     = NULL;
    char    *res_str = NULL;

    if (num > 0) {
        res_str = atoi(num/10);
        while(*res_str != '\0') {
            *str++ = *res_str++;
        }
        *str++ = num % 10;
        *str = '\0';
    }
    else {
        *str = '\0';
    }
    return str;
    kprintf("VALUE NOW IS this much value you know : %s",str);
*/
}

void  convertHex2String(int n){
    char string[20];
    char * string2 = "0x";
    int i =0;
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
    int j =2;
    while(i >= 0  ){
      string2[j++] = string[i--];
    }
    string2[j] = '\0';
    i =0;
    while(string2[i] != '\0') {
                    *video++ = string2[i]++;
                    *video++ = colour;
     i++;
    }
}

void  convertPointerAddress2String(unsigned long n){
    char string[50];
    char *string2 = "0x";
    int i =0;
    while (n >0){
        int rem1 = n %16;
        long div = n/16;
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
    int j =2;
    while(i >= 0  ){
        string2[j++] = string[i--];
    }
    string2[j] = '\0';
    i =0;
    while(string2[i] != '\0') {
         *video++ = string2[i];
         *video++ = colour;
         i++;
     }
}
void kprintf(const char *string, ...)
{
    va_list       list;
    va_start(list, string);
//    char         *video     = ( char*)0xB8000;
 //   int           colour    =  31;
    char          char_val;
    int           int_val   = 0;
    unsigned long long_val;
    char         *str_val   = NULL;

    while( *string != 0)
    {
        while (*string != '%') {
            *video++ = *string++;
            *video++ = colour;
        }
        string++;

        switch(*string) {
            case 'c':
                char_val = va_arg(list, int);
                *video++ = char_val;
                *video++ = colour;
                break;
            case 's':
                str_val = va_arg(list, char *);
                while (*str_val != '\0') {
                    *video++ = *str_val++;
                    *video++ = colour;
                }
                break;
            case 'd':
                int_val = va_arg(list, int);
//                str_val = atoi(int_val);
                while(*str_val != '\0') {
                    *video++ = *str_val++;
                    *video++ = colour;
                }
                break;
           case 'p' :
                  long_val = va_arg(list, unsigned long);
                  convertPointerAddress2String(long_val);
                break;
          case 'x' :
		 int_val = va_arg(list, int);
                 convertHex2String(int_val);
                		break;
        }
        string++;

    }
    return;
}
#endif    
