#include <sys/picassem.h>
#include <sys/kprintf.h>
#include <sys/pic.h>
#include <sys/idt.h>

/*Keeps track of the number of ticks for which the system runs */
int timer_tic = 0;

void timer_count(int hz){
    int div = 1193180/hz;
    outb(0x43,0x36);    //command regsiter at 0x43 and command = 0x36 
    outb(0x40, div & 0xFF); //sending upper byte of divisor 
    outb(0x40, div >> 8);   //seding  lower bytes 
}

void timer_handler(struct regis *r){
   /*Increase the count */
    timer_tic++;
    outb(0x20,0x20);
    outb(0xa0,0x20);
    /*Display after every 100 clocks */
    if(timer_tic % 1000 == 0){
      //kprintf("%d\n",timer_tic/1000);
        kprintf_timer(timer_tic/1000);
    }
}

/* Start the timer */
void init_timer(int hz){
    timer_count(hz);
    irq_set(32, timer_handler);
}

