#ifndef  _LIBC_H_
#define  _LIBC_H_

char ls_str[1000];
char global_str[1000];

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
#endif	
