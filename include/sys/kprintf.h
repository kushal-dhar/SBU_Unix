#ifndef __KPRINTF_H
#define __KPRINTF_H

void kprintf(char *fmt, ...);
void kprintf_kb(char ch);
void kprintf_timer(int time);
void update_screen();
void clear_console();
extern void full_clean();

#endif
