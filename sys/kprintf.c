#include <sys/kprintf.h>
#include <stdarg.h>
#include <sys/defs.h>
#include <sys/kb.h>
#include <sys/paging.h>

volatile char *video = ( char*)VIDEO_ADDR;
int            colour=  7;

volatile int x_pos = 0;
volatile int y_pos = 0;
unsigned char control;
unsigned char shift;
void  memcpyy( volatile void *src, volatile void *dest, uint32_t n) {
    char *temp1 = (char *)src;
    char *temp2 = (char *)dest;
    int i = 0;

    for (i = 0; i < n/2; i++) {
        temp2[i] = temp1[i];
    }   

}


void strtoint (int num, char str[])
{
    int i = 0;

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

/*
void update_screen() {
    int       x       = 0;
    int       y       = 0;
    char     *mem_loc = (char *)0xB8000;

    for (x = 0; x < 160; x++) {
        for (y = 0; y < 24; y++) {
             mem_loc[x + (y*160)] = mem_loc[x + ((y+1)*160)];
             mem_loc[x + (y*160)] = mem_loc[x + ((y+1)*160)];
        }
    }
}*/

void update_screen() {
volatile     int y =0;
     for (y = 0; y < 24; y++) {
       memcpyy((video+ ((y+1)*160)),(video+ (y*160)), 320);   
        }
}

void kprintf(char *string, ...)
{
    va_list       list;
    va_start(list, string);
    char          char_val;
    int           int_val   = 0;
    char         *str_val   = NULL;
    char          str[50];
    int           i         = 0;
    unsigned long long_val = 0;
    while( *string != '\0')
    {
        while (*string != '%' && *string != '\0') {
            if (*string == '\n') {
                y_pos ++;
                x_pos = 0;
                string ++;
                if (y_pos == 24) {
                    update_screen();
                    y_pos --;
                }
            }
            else if (*string == '\r') {
                x_pos = 0;
                string ++;
            } else {
                if (x_pos == 160) {
                    x_pos = 0;
                    y_pos ++;
                }
                if (y_pos == 24) {
                    update_screen();
                    y_pos --;
                }
                video[x_pos++ + (y_pos*160)] = *string++;
                video[x_pos++ + (y_pos*160)] = colour;
            }
        }
        if (*string == '%') {
	    string++;
            switch(*string) {
                case 'c':
                    char_val = va_arg(list, int);
                    if (x_pos == 160) {
                        x_pos = 0;
                        y_pos ++;
                        if (y_pos == 24) {
                            update_screen();
                            y_pos --;
                        }
                    }
 		    /*
		     * Handle the case separately if character contains only \n
		     */
		    if (char_val != '\n') {
                        video[x_pos++ + (y_pos*160)] = char_val;
                        video[x_pos++ + (y_pos*160)] = colour;
 		    } else {
	                y_pos ++;
        	        x_pos = 0;
	                if (y_pos == 24) {
        	            update_screen();
                	    y_pos --;
	                }
		    }
                    break;
                case 's':
                    str_val = va_arg(list, char *);
                    while (*str_val != '\0') {
                        if (x_pos == 160) {
                           x_pos = 0;
                           y_pos ++;
                        }
                        if (y_pos == 24) {
                            update_screen();
                            y_pos --;
                        }
                        video[x_pos++ + (y_pos*160)] = *str_val++;
                        video[x_pos++ + (y_pos*160)] = colour;
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
                        if (x_pos == 160) {
                            x_pos = 0;
                            y_pos ++;
                        }
                        if (y_pos == 24) {
                            update_screen();
                            y_pos --;
                        }
                        video[x_pos++ + (y_pos*160)] = str[i++];
                        video[x_pos++ + (y_pos*160)] = colour;
                    }
                    break;
                case 'x' :
                    int_val = va_arg(list, int);
                    i = 0;
                    convertHex2String(int_val, str);
                    while(str[i] != '\0') {
                        if (x_pos == 160) {
                            x_pos = 0;
                            y_pos ++;
                        }
                        if (y_pos == 24) {
                            update_screen();
                            y_pos --;
                        }
                        video[x_pos++ + (y_pos*160)] = str[i++];
                        video[x_pos++ + (y_pos*160)] = colour;
                    }
                    break;
                case 'p' :
                    long_val = (unsigned long)va_arg(list, unsigned long);
                    convertPointerAddress2String(long_val, str);
		    i = 0;
                    while(str[i] != '\0') {
                        if (x_pos == 160) {
                            x_pos = 0;
                            y_pos ++;
                        }
                        if (y_pos == 24) {
                            update_screen();
                            y_pos --;
                        }
                        video[x_pos++ + (y_pos*160)] = str[i++];
                        video[x_pos++ + (y_pos*160)] = colour;
                    }
                    break;

	        default:
		    break;
	    }
            string++;
	}
    }
    return;
}
        

void kprintf_kb(char ch) {
    int  x = 80;

    if (ch == '\n') {
        video[x++ + (24 * 160)] = '^';
        video[x++ + (24 * 160)] = colour;
        video[x++ + (24 * 160)] = 'M';
        video[x++ + (24 * 160)] = colour;
        video[x++ + (24 * 160)] = ' ';
        video[x++ + (24 * 160)] = colour;
        video[x++ + (24 * 160)] = ' ';
        video[x++ + (24 * 160)] = colour;
    }
    else if (control == 1) {
	if (ch == 0x48 && shift == 1) {
	/* Up arrow has been pressed, display '^[[A' */
            video[x++ + (24 * 160)] = '^';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = '[';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = '[';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = 'A';
            video[x++ + (24 * 160)] = colour;
	}
	else if(ch == 0x4B && shift == 1) {
	   /* Left arrow has been pressed, display '^[[D' */
            video[x++ + (24 * 160)] = '^';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = '[';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = '[';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = 'D';
            video[x++ + (24 * 160)] = colour;
	}
	else if (ch == 0x4D && shift == 1) {
	    /* Right arrow has been pressed, display '^[[C' */
            video[x++ + (24 * 160)] = '^';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = '[';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = '[';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = 'C';
            video[x++ + (24 * 160)] = colour;
	}
	else if (ch == 0x50 && shift == 1) {
	    /* Down arrow has been pressed, display '^[[B' */
            video[x++ + (24 * 160)] = '^';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = '[';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = '[';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = 'B';
            video[x++ + (24 * 160)] = colour;
	}
	else if (ch == 0x68) {
	    /* Backspace has been pressed, display ^H */
            video[x++ + (24 * 160)] = '^';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = 'H';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = ' ';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = ' ';
            video[x++ + (24 * 160)] = colour;
        }
	else {
	    /* For all other control characters, display their Ctrl-characters */
            video[x++ + (24 * 160)] = '^';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = ch;
            video[x++ + (24 * 160)] = colour;	
            video[x++ + (24 * 160)] = ' ';
            video[x++ + (24 * 160)] = colour;
            video[x++ + (24 * 160)] = ' ';
            video[x++ + (24 * 160)] = colour;
	}
    }
#if 0
    else if (ch == 30) {
        video[x++ + (24 * 160)] = '^';
        video[x++ + (24 * 160)] = colour;
        video[x++ + (24 * 160)] = 'C';
        video[x++ + (24 * 160)] = colour;
    } 
    else if (ch == 31) {
        video[x++ + (24 * 160)] = '^';
        video[x++ + (24 * 160)] = colour;
        video[x++ + (24 * 160)] = 'Z';
        video[x++ + (24 * 160)] = colour;
    }
#endif
    else {
        video[x++ + (24 * 160)] = ch;
        video[x++ + (24 * 160)] = colour;
        video[x++ + (24 * 160)] = ' ';
        video[x++ + (24 * 160)] = colour;
        video[x++ + (24 * 160)] = ' ';
        video[x++ + (24 * 160)] = colour;
        video[x++ + (24 * 160)] = ' ';
        video[x++ + (24 * 160)] = colour;
    }

    return;
}


void kprintf_timer(int time) {
    int    hr    = 0;
    int    mins  = 0;
    int    secs  = 0;
    volatile int    x     = 110;
    int    i     = 0;
    char  *str   = "Time since boot: ";
    char   str_t[50];

    hr = time / 3600;
    time = time % 3600;
    mins = time / 60;
    secs = time % 60;
    
    /* Display the string for time */
    while (*str != '\0') {
	video[x++ + (24 * 160)] = *str++;
        video[x++ + (24 * 160)] = colour;
    }

    /* Display the time in hour */
    while(str_t[i] != '\0') {
        str_t[i++] = '\0';
    }
    i = 0;
    strtoint(hr, str_t);
    while(str_t[i] != '\0') {
        video[x++ + (24 * 160)] = str_t[i++];
        video[x++ + (24 * 160)] = colour;
    }
    video[x++ + (24 * 160)] = ':';
    video[x++ + (24 * 160)] = colour;

    
    /* Display the time in minutes */
    i = 0;
    while(str_t[i] != '\0') {
        str_t[i++] = '\0';
    }
    i = 0;
    strtoint(mins, str_t);
    while(str_t[i] != '\0') {
        video[x++ + (24 * 160)] = str_t[i++];
        video[x++ + (24 * 160)] = colour;
    }
    video[x++ + (24 * 160)] = ':';
    video[x++ + (24 * 160)] = colour;

    /* Display the time in minutes */
    i = 0;
    while(str_t[i] != '\0') {
        str_t[i++] = '\0';
    }
    i = 0;
    strtoint(secs, str_t);
    while(str_t[i] != '\0') {
        video[x++ + (24 * 160)] = str_t[i++];
        video[x++ + (24 * 160)] = colour;
    }
    while (x <=160) {
        video[x++ + (24 * 160)] = ' ';
        video[x++ + (24 * 160)] = colour;
    }

}


