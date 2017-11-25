#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/timer.h>
#include <sys/kb.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <sys/process.h>
#include <sys/elf64.h>
#include <sys/syscalls.h>

/* Start syscall handler */
void init_syscalls() {
    irq_set_with_return(128, syscall_handler);
}

int syscall_handler(regis *r) {
    uint64_t   syscall_no;
    uint64_t   buf;
    uint64_t   rcx;
    uint64_t   rdx;
//    uint64_t   size; 
    
    __asm__ volatile("movq %%rax, %0;" 
                     "movq %%rbx, %1;"
	             "movq %%rcx, %2;"
	  	     "movq %%rsi, %3;"
		     : "=r"(syscall_no), "=r"(buf), "=r"(rcx), "=r"(rdx)
		     :
		     : "rax", "rsi", "rcx"
		     );
    kprintf("I am able to parse this %d  %d  %d  %d\n",syscall_no,buf,rcx,rdx);

    return 10;
}
