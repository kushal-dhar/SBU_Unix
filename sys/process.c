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

extern uint64_t *pml4;
uint64_t *old_rsp;
uint64_t *rsp_addr;
uint64_t processID = 0;
//uint64_t *user_virtual_address = (uint64_t*)0x88888fff80000000UL;
pcb_t *first_process;
pcb_t *curr_process;
pcb_t *next_process;


/* 
 * Get the next available process ID
 */
int get_next_processID() {
    int id = 1;
    int i = 0;

    for (i = 0; i < MAX_PROCESS_COUNT; i++) {
	if (((processID >> i) & id) == 0x0){
	    processID |= (id << i);
	    return i+1;
	}
    }
    return -1;
}

/*
 * Create PCB Structure and populate all its entries 
 * for the currently executing kernel process.
 */
void create_kernel_thread() {
    pcb_t *k_pcb = (pcb_t *)kmalloc((int)6000);
    
    k_pcb->pid = get_next_processID();
    k_pcb->ppid = 0;
    k_pcb->cr3 = (uint64_t)get_CR3_address();
    k_pcb->state = TASK_RUNNING;
    k_pcb->next_proc = next_process;
    k_pcb->mm = NULL;
    //k_pcb->rip = (uint64_t)&initial_ret_function;

    first_process = (pcb_t *)k_pcb;
    curr_process = (pcb_t *)k_pcb;
    
    k_pcb->rsp = k_pcb->init_kernel = ((uint64_t)k_pcb + PAGE_SIZE);
    k_pcb->rsp -= 8;
    *(uint64_t *)(k_pcb->rsp) = (uint64_t)&initial_ret_function;

    create_new_thread();

    /* Load the current PCB and kernel stack and disable interrupt */
    __asm__ volatile ("cli");
    __asm__ volatile ("movq %0, %%rsp;" :: "r"((k_pcb->init_kernel)));

    scheduler();

    initial_ret_function();

    return;
}

/*
 * Create PCB Structure and populate all its entries
 * for the currently executing kernel process.
 */
void create_new_thread() {
    pcb_t *child_pcb = (pcb_t *)kmalloc((int)6000);
    next_process = (pcb_t *)child_pcb;

    child_pcb->pid = get_next_processID();
    child_pcb->ppid = 0;
//    strcpy(k_pcb->p_name, "main thread");
    child_pcb->cr3 = (uint64_t)get_CR3_address();
    child_pcb->state = TASK_READY;
    child_pcb->next_proc = curr_process;
    child_pcb->mm = NULL;
    child_pcb->rip = (uint64_t)&test_function;

    child_pcb->rsp = child_pcb->init_kernel = ((uint64_t)child_pcb + PAGE_SIZE);
    child_pcb->rsp  -= 8;
    *(uint64_t *)(child_pcb->rsp) = (uint64_t)&test_function;
//    *(uint64_t *)(child_pcb->rsp) |= KERNEL_ADDR;
    child_pcb->regs.rax = child_pcb->regs.rbx = child_pcb->regs.rdi = child_pcb->regs.rsi = child_pcb->regs.rbp = 0;
    
    return;
}


/* 
 * Create PCB for user process and loads the same process
 */
pcb_t* create_user_process(char *filename) {
    pcb_t *user_pcb = (pcb_t *)kmalloc((int)6000);
//    uint64_t     offset       = 0;
    uint64_t    *user_virt;
    uint64_t    *user_stack;
    uint64_t    *file_pt;
    int          ret_val;
    mm_struct_t *mm;

    user_pcb->pid = get_next_processID();
    user_pcb->ppid = 0;
    user_pcb->cr3 = (uint64_t)create_user_address_space();
//    user_virt = (uint64_t *)((uint64_t)USER_VIRT_ADDR | user_pcb->cr3);

    user_stack = (uint64_t *)allocate_virt_page();
    user_virt = (uint64_t *)((uint64_t)USER_VIRT_ADDR | (uint64_t)user_stack);
    map_phys_to_virt_addr((uint64_t)user_virt, (uint64_t)user_stack);
    map_phys_to_user_virt_addr((uint64_t)user_virt, (uint64_t)user_stack, (uint64_t *)user_pcb->cr3);
    set_CR3((uint64_t)user_pcb->cr3);
    memset((void*)user_virt, 0, (uint32_t)PAGE_SIZE);
    set_CR3((uint64_t)first_process->cr3);

//    mm = (mm_struct_t *)kmalloc(1000);
    mm = (mm_struct_t *)allocate_virt_page();
    user_virt = (uint64_t *)((uint64_t)USER_VIRT_ADDR | (uint64_t)mm);
    map_phys_to_virt_addr((uint64_t)user_virt, (uint64_t)mm); 
    map_phys_to_user_virt_addr((uint64_t)user_virt, (uint64_t)mm, (uint64_t *)user_pcb->cr3);
    set_CR3((uint64_t)user_pcb->cr3);
    memset((void*)user_virt, 0, (uint32_t)PAGE_SIZE);
    user_pcb->mm = (mm_struct_t *)user_virt;

    file_pt = (uint64_t *)is_file_exist(filename);
    ret_val = load_binaries(user_pcb, (uint64_t *)file_pt);
    if (ret_val == -1) {
	kprintf("Unable to load binaries\n");
	return NULL;
    }

    set_CR3(first_process->cr3);

/*
    user_pcb->rip = (uint64_t)&test_user_thread;
    offset = user_pcb->rip & 0xfff;
*/
    user_pcb->state = TASK_READY;
    user_pcb->next_proc = first_process;

#if 0
    uint64_t *user_page = (uint64_t *)allocate_virt_page();
    user_virt_addr = (uint64_t *)((uint64_t)user_virtual_address | (uint64_t)user_page);
    map_phys_to_user_virt_addr((uint64_t)user_virt_addr, (uint64_t)user_page, (uint64_t *)user_pcb->cr3);
    set_CR3((uint64_t)user_pcb->cr3);
    memset((void*)user_virt_addr, 0, (uint32_t)PAGE_SIZE);
    set_CR3((uint64_t)pml4);
#endif

//    uint64_t *kstack = (uint64_t *)umalloc(4096, (uint64_t *)user_pcb->cr3);
    user_pcb->rsp = user_pcb->init_kernel = ((uint64_t)user_virt + PAGE_SIZE - 8);

//    set_user_space(user_pcb, offset);

//    set_CR3((uint64_t)user_pcb);
//    kprintf("I just loaded new CR3\n");

    return user_pcb;
}


/*
 * Initial return function for pcb 1 kernel thread.
 */
void initial_ret_function() {
    __asm__ volatile("sti");

    init_idt();

    init_picirr();

    init_timer(1000);
    kb_init();
    int  i = 0;
    while (i < 4) {
        kprintf("I am in kernel thread again\n");
        switchTask(first_process, next_process);
        i ++;
    }

//    init_picirr();

//    init_timer(1000);
//    kb_init();

    init_syscalls();
    
    pcb_t *user_process = create_user_process("bin/hello");
    switch_to_ring3((pcb_t *)user_process);

//    pcb_t *user_process = create_user_process();
//    switch_to_ring3((pcb_t *)user_process);
    while(1);
}


/* 
 * Test function to check new kernel thread
 */
void test_function() {
//	switchBack(next_process, first_process);
    int i = 0;
    while(i < 5) {
	kprintf("I am in thread 1 after context switch\n");        
	switchTask(next_process, first_process);
	i++;
    }
//    while(1);
}

/* Dummy function to test user thread */
void test_user_thread() {
    uint64_t a = 2;
    uint64_t b = 3;
    uint64_t c = 5;
//    uint64_t val;
    
//    kprintf("Trying to print something\n");
//    __asm__ volatile("int $0x80");
//    __asm__ volatile("movq %%rax, %0;" : "=r"(b));
    __asm__ volatile("movq %0, %%rax;"::"r"(c));
    __asm__ volatile("movq %0, %%rbx;"::"r"(a));
    __asm__ volatile("movq %0, %%rcx;"::"r"(b));
//    __asm__ volatile("movq %0, %%rsi;"::"r"(c));
    __asm__ volatile("int $0x80");
/*    __asm__ volatile("movq %1, %%rax;"
		     "movq %2, %%rbx;"
		     "int $0x80;"
		     "movq %%rax, %0;"
		     : "=r" (val)
		     : "r"(a), "r"(b)
		     : "rbp", "rcx", "rdx", "rsi", "rdi"
		     ); */
//    kprintf("val is now: %d\n",val);
    while(1);
}


void scheduler() {
//    first_process = curr_process;
//    curr_process = next_process;
//    rsp_addr = (uint64_t *)(next_process->init_kernel - 128);
    switchTask(curr_process, next_process);
}


/*
 * Process to switch from 1st to 2nd process
 */
void switchTask(pcb_t *current, pcb_t *next) {

//    __asm__ volatile ("cli"); 
/*
    __asm__ volatile ("movq %%rsp, %0;" : "=r"((current->init_kernel)));
    current->init_kernel += 8;
    __asm__ volatile ("movq %0, %%rsp;" :: "r"((next->init_kernel)));
    __asm__ volatile ("movq %0, %%rax"::"r" (next->rip));
    __asm__ volatile ("pushq %%rax;" :: );
    __asm__ volatile ("movq %%rsp, (%0)":"=r"(rsp_addr));

    next->init_kernel = (uint64_t)rsp_addr;
    set_tss_rsp((void *)rsp_addr);

    __asm__ volatile ("ret");
*/
//    save_registers(current);

    __asm__ volatile("movq %%rdi, %0;" : "=r"(current->regs.rdi));
    __asm__ volatile("movq %%rsi, %0;" : "=r"(current->regs.rsi));
    __asm__ volatile("movq %%rax, %0;" : "=r"(current->regs.rax));
    __asm__ volatile("movq %%rbx, %0;" : "=r"(current->regs.rbx));
    __asm__ volatile("movq %%rbp, %0;" : "=r"(current->regs.rbp));

//    save_rsp(current);
    __asm__ volatile ("movq  %%rsp, %0;":"=r"(current->rsp));
    __asm__ volatile ("movq %0, %%rsp;"::"r"(next->rsp));

    __asm__ volatile ("movq %0, %%rdi;" :: "r"(next->regs.rdi));
    __asm__ volatile ("movq %0, %%rax;" :: "r"(next->regs.rax));
    __asm__ volatile ("movq %0, %%rbx;" :: "r"(next->regs.rbx));
    __asm__ volatile ("movq %0, %%rbp;" :: "r"(next->regs.rbp));
    __asm__ volatile ("movq %0, %%rsi;" :: "r"(next->regs.rsi));
//    update_registers(next);

}

/*
 * Process to switch back from 2nd to 1st process
 */
void switchBack(pcb_t *current, pcb_t *next) {
    
    __asm__ volatile ("movq %%rsp, %0;" : "=r"((current->init_kernel)));
    __asm__ volatile ("movq %0, %%rsp;" :: "r"((next->init_kernel)));

    next->init_kernel = (uint64_t)next->init_kernel;
    set_tss_rsp((void *)next->init_kernel);

    __asm__ volatile ("ret");

}

/*
 * Stub to switch ring 3
 */
void switch_to_ring3(pcb_t *proc) {
    proc->state = TASK_RUNNING;
    curr_process = proc;
    

    set_tss_rsp((void *) first_process->init_kernel);

    __asm__ volatile ("cli" ::);
    __asm__ volatile ("movq %0, %%cr3;" :: "r"(proc->cr3));
    __asm__ volatile ("mov $0x23, %%ax" ::);
    __asm__ volatile ("mov %%ax, %%ds;" ::);
    __asm__ volatile ("mov %%ax, %%es;" ::);
    __asm__ volatile ("mov %%ax, %%fs;" ::);
    __asm__ volatile ("mov %%ax, %%gs;" ::);
    __asm__ volatile ("movq %0, %%rax;"::"r"(proc->rsp));
    __asm__ volatile ("pushq $0x23" ::);
//    __asm__ volatile ("movq %%rsp, %0;" : "=r"(proc->rsp));
    __asm__ volatile ("pushq %%rax" ::);
    __asm__ volatile ("pushfq" ::);
    __asm__ volatile ("popq %%rax" ::);
    __asm__ volatile ("orq $0x200, %%rax;" ::);
    __asm__ volatile ("pushq %%rax" ::); 
    __asm__ volatile ("pushq $0x2B" ::);
    __asm__ volatile ("pushq %0;" :: "r"(proc->rip));
    __asm__ volatile ("movq $0x0, %%rdi" ::);
    __asm__ volatile ("movq $0x0, %%rsi" ::);
    __asm__ volatile ("iretq" ::);
//    __asm__ volatile ("sti" ::);


}

void set_user_space(pcb_t *user_process, uint64_t offset) {
    uint64_t* user_page = (uint64_t *)user_process->rip;
    uint64_t* user_virtual_addr;

//    user_virtual_address += 0x1000UL;
    user_page = (uint64_t *)(0x000000000FFFF000UL & (uint64_t)user_page);
    user_virtual_addr = (uint64_t *)((uint64_t)USER_VIRT_ADDR| (uint64_t)user_page);
    
    map_phys_to_virt_addr((uint64_t)user_virtual_addr, (uint64_t)user_page);
    map_phys_to_user_virt_addr((uint64_t)user_virtual_addr, (uint64_t)user_page, (uint64_t *)user_process->cr3);
    user_process->rip = (uint64_t)user_virtual_addr | offset;

}
