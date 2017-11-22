/*
 * The lowercase keyboard scancode has been taken from:
 * http://www.osdever.net/tutorials/view/brans-kernel-development-tutorial
 */


#include <sys/kprintf.h>
#include <sys/pic.h>
#include <sys/picassem.h>
#include <sys/idt.h>
#include <sys/kb.h>


#define KB_DATA_PORT   0x60
#define KB_STATUS_PORT 0x64

unsigned char control = 0;
unsigned char shift   = 0;

unsigned char keyboard_map[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
  '9', '0', '-', '=', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
   13 ,                  /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
 '\'', '`',   15,                /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
  'm', ',', '.', '/',   15,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

unsigned char keyboard_upper_map[] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',     /* 9 */
  '(', ')', '_', '=', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',     /* 39 */
 '\'', '~',   15,                /* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',                    /* 49 */
  'M', '<', '>', '?',   0,                              /* Right shift */
  '*',
};


void kb_handler(struct regis *s) {
    unsigned char status;
    unsigned char scancode;

    /* Acknowledge Interrupt by writing to PIC's command port */
    outb(0x20, 0x20);

    status = inb(KB_STATUS_PORT);
    /* Lower bit of status will be set if keyboard is pressed */
    if (status & 0x01) {
        scancode = inb(KB_DATA_PORT);
        if (scancode < 0) {
	    return;
	}
	if (scancode == 0x2A || scancode == 0x36) {
	/* Shift key has been pressed, update the global variable */
	    shift = 1;
	    return;
	}
	else if (scancode == 0xAA || scancode == 0xB6) {
	/* Shift key has been released, update the global variable again */
	    shift = 0;
	    return;
	}
	else if (scancode == 0x1D) {
	/* Control key has been pressed, update the global variable */
	    control = 1;
	    return;
	}
	else if(scancode == 0x9D) {
	/* Control key has been released, update the global variable */
	    control = 0;
	    return;
	}
	else if(scancode == 0x48) {
	/* Up arrow has been pressed, display '^[[A' */
	    control = 1;
	    shift = 1;
	    kprintf_kb(0x48);
	    control = 0;
	    shift = 0;
	    return;
	}
        else if(scancode == 0x4B) {
        /* Left arrow has been pressed, display '^[[D' */
            control = 1;
	    shift = 1;
            kprintf_kb(0x4B);
            control = 0;
	    shift = 0;
	    return;
        }
        else if(scancode == 0x50) {
	/* Down arrow has been pressed, display '^[[B' */
            control = 1;
            shift = 1;
            kprintf_kb(0x50);
            control = 0;
            shift = 0;
	    return;
        }
        else if(scancode == 0x4D) {
	/* Right arrow has been pressed, display '^[[C' */
            control = 1;
            shift = 1;
            kprintf_kb(0x4D);
            control = 0;
            shift = 0;
	    return;
        }
	else if (scancode == 0x0E) {
	/* If backspace is pressed, diplay '^H' */
	    control = 1;
	    kprintf_kb('h');
	    control = 0;
	    return;
	}
	else if (scancode == 0x53) {
	/* Delete has been pressed, display ^? */
	    control = 1;
	    kprintf_kb('?');
	    control = 0;
	    return;
	}
	else if (scancode > 127) {
	    return;
	}
        if (shift == 1 || control == 1) {
	    /* If shift is pressed previously, read from the upper keyboard map */
            status = keyboard_upper_map[(int)scancode];
   	    kprintf_kb(status);
	    //shift = 0;
#if 0
	} else if (control == 1) {
	    /* Incase if control is pressed, look for the next character and send the 
	       appropriate character */
	    status = keyboard_map[(int)scancode];
	    if (status == 'c') {
      	        kprintf_kb(30);
            } else if (status == 'z') {
		kprintf_kb(31);
	    }
	    control = 0;	
#endif
	} else {
	    status = keyboard_map[(int)scancode];
/*	    if (status == 13) {
		control = 1;
		return;
	    } */
	    kprintf_kb(status);
	}
    }
    return;
}

/* Initialize keyboard handler */
void kb_init(void) {
    outb(0x21 , 0xFC);
    irq_set(33, kb_handler);
}

