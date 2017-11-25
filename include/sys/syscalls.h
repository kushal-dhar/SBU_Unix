#ifndef _SYSCALLS_H
#define _SYSCALL_H

extern void init_syscalls();
extern int syscall_handler(regis *r);

#endif
