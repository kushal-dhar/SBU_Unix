/*
 * The asm codes has been taken unchanged from:
 * http://wiki.osdev.org/Expanded_Main_Page
 */


#include <sys/picassem.h>
#include <sys/defs.h>

void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

uint8_t inb(uint16_t port)
{
    uint8_t ret;
   __asm__ volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void io_wait(void)
{
    __asm__ volatile ( "outb %%al, $0x80" : : "a"(0) );
}




