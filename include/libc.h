#ifndef  _LIBC_H_
#define  _LIBC_H_

#include <sys/defs.h>

char ls_str[1000];
char global_str[1000];
uint64_t global_val;
int temp_val;
char sys_temp[100];
extern char*  read_sys(long syscall, long arg1, long arg2, long arg3);
extern long  write_sys(long syscall, long arg1, long arg2, long arg3);
extern long syscall_wait(long syscall, long arg1, int *arg2, long arg3);
extern unsigned int syscall_execve(long syscall, char *file, char *argv[]);
extern long syscall_open(long syscall, long arg1, long arg2);
extern long syscall_dup(long syscall, long arg1, long arg2);
extern long syscall_close(long syscall, long arg1);
extern long syscall_pipe(long syscall, long* arg1);
extern long syscall_fork(long syscall, long arg1);
extern long syscall_dirent(long syscall, long fd, char *buf, long count);
extern char* read_char(long syscall, long arg1, char *arg2, long arg3);
extern long write_char(long syscall, long arg1, char *arg2, long arg3);
extern uint64_t syscall_getpid(uint64_t syscall);
extern uint64_t syscall_1(uint64_t syscall);
extern uint64_t syscall_2(uint64_t syscall, uint64_t ch);
extern void syscall_3(uint64_t syscall, uint64_t ch, uint64_t buf);
extern void syscall_4(uint64_t syscall, uint64_t ch, uint64_t buf, uint64_t size);
extern void syscall_2next(uint64_t syscall);
extern void syscall_2_1(uint64_t syscall, uint64_t ch);
extern void syscall_2_char(uint64_t syscall);
extern uint64_t syscall_pid();
#endif	
