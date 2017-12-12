#include <sys/picassem.h>
#include <sys/kprintf.h>
#include <sys/pic.h>
#include <sys/idt.h>

/*Keeps track of the number of ticks for which the system runs */
volatile int timer_tic = 0;
volatile int current_sec = 0;
volatile int sleep_flag = 0;
void timer_count(int hz){
    int div = 1193180/hz;
    outb(0x43,0x36);    //command regsiter at 0x43 and command = 0x36 
    outb(0x40, div & 0xFF); //sending upper byte of divisor 
    outb(0x40, div >> 8);   //seding  lower bytes 
}

void timer_handler(regis reg){
   /*Increase the count */
    timer_tic++;
    outb(0x20,0x20);
    outb(0xa0,0x20);
    /*Display after every 100 clocks */
    if(timer_tic % 1000 == 0){
        //kprintf("%d\n",timer_tic/1000);
        if (sleep_flag == 0){
            kprintf_timer(timer_tic/1000);
        }
        current_sec = ((timer_tic/1000) % 3600) % 60;
    }
}

/* Start the timer */
void init_timer(int hz){
    timer_count(hz);
    irq_set(32, timer_handler);
}

void sleep(int val ){
      volatile int cur_tic = timer_tic ;
      sleep_flag = 1;
      volatile int ti  = current_sec;
      ti = ti+val;
      while(current_sec < ti);
      sleep_flag =0;
      timer_tic = cur_tic; 
}
