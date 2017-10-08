#ifndef _TIMER_H
#define _TIMER_H

#include <sys/idt.h>

void timer_count(int hz);
void timer_handler(struct regis *r);
void init_timer(int hz);

#endif
