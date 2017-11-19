
/* 
 * Finds the length of string
 */
int strlen(char *str) {
    int i = 0;
    while (*str != '\0') {
	i++;
	str++;
    }
    return i;
}

/*
 * Copy string src to string dst
 */
char *strcpy(char *src, char *dst) {
    char *temp = (char *)dst;
    while (*src != '\0') {
	*temp++ = *src++;
    }
    *temp = '\0';
    return dst;
}

/* 
 * Compares string str1 and str2
 */
int strcmp(char *str1, char *str2) {

    while (*str1 != '\0' && *str2 != '\0') {
	if (*str1 != *str2) {
	    return 1;
	}
	str1++;
	str2++;
    }
    if (*str1 != '\0' || *str2 != '\0') {
	return 1;
    }
    return 0;
}

int atoi(char *str) {
    int val = 0;
    while (*str != '\0') {
	val = (val * 10) + (*str - '0');
	str++;
    }
    return val;
}
