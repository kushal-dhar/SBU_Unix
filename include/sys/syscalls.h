#ifndef _SYSCALLS_H
#define _SYSCALL_H

extern void init_syscalls();
extern uint64_t syscall_handler(regis * reg);

#endif
