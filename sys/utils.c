
/*
 * Returns the power of certain base to its exponent
 */ 
int pow(int base, int exp) {
    int val = 0;

    if (exp == 0) {
	return 1;
    }
    val = base * pow(base, exp -1);

    return val;
}

/*
 * Convert octal value to decimal values
 */
int octtodec(int octal_val) {
    int    int_val     = 0;
    int    pow_eight   = 0;
    
    while (octal_val > 0) {
	int_val += (octal_val % 10) * pow(8, pow_eight);
	octal_val /= 10;
	pow_eight ++;
    }
    return int_val;
}
