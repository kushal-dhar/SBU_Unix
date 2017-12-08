#include <sys/defs.h>
#include <sys/pic.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/idt.h>
#include <sys/timer.h>
#include <sys/kb.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <sys/process.h>
#include <sys/elf64.h>
#include <sys/tarfs.h>
#include <sys/syscalls.h>

/* Start syscall handler */
void init_syscalls() {
    irq_set_with_return(128, syscall_handler);
}

uint64_t syscall_handler (regis * reg) {
#if 0
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
#endif
    /* Handle user printf */
    uint64_t  retV =0;
    if (reg->rbx == 1) {
        kprintf("%s",reg->rcx);
	return 0;
    }
    /* Handle user scanf */
    else if (reg->rbx == 4) {
	__asm__ volatile("sti");
	scanf((char *)reg->rcx);
	return 0;
    }
    /* Handle read_dir */
//    else if (reg->rbx == 39) {
//	read_dir((int)reg->rcx);
//	return 0;
 //   }
    /* Handle open dir */
    else if (reg->rbx == 89) {
	int ret_val = opendir((char *)reg->rcx);
	return ret_val;
    }
    /* Handle read dir */
    else if (reg->rbx == 90) {
	read_dir((int)reg->rcx);
	return 0;
    }
    /* Handle open file */
    else if (reg->rbx == 0) {
	int ret_val;
	ret_val = open((char *)reg->rcx, (int)reg->rdx);
	return ret_val;
    }
    /* Handle read files */
    else if (reg->rbx == 2) {
	read((int)reg->rcx, (char *)reg->rdx, (int) reg->rdi);
	//kprintf("%s\n",(char*)reg->rdx);
	return 0;
    }
    /* Handle chdir syscall */
    else if (reg->rbx == 80){
       int ret_val = 0;
       chdir((char*)reg->rcx);
       return ret_val;
    }
    /* Handle getcwd syscall */
    else if (reg->rbx == 79){
       getcwd();
       return 0;
    }
    /* Handle clear console */
    else if (reg->rbx == 91) {
	clear_console();
    }
    /* Handle mmap call*/
     else if (reg->rbx == 9){
        retV = mmap((int) reg->rcx);
        return retV;
    }
    /* Handle getPid calls*/
    else if (reg->rbx == 39){
       retV = get_pid();
       return retV;
    }
    return 0;
}
