#ifndef _TIMER_H
#define _TIMER_H

#include <sys/idt.h>

void timer_count(int hz);
void timer_handler();
void init_timer(int hz);
void sleep(int val);
#endif
