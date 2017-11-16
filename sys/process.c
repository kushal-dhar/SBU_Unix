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

uint64_t *old_rsp;
uint64_t *rsp_addr;
uint64_t processID = 0;
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
    
    return;
}


/* 
 * Create PCB for user process and loads the same process
 */
pcb_t* create_user_process() {
    pcb_t *user_pcb = (pcb_t *)kmalloc((int)6000);
    uint64_t     offset = 0;
    mm_struct_t *vma;

    user_pcb->pid = get_next_processID();
    user_pcb->ppid = 0;
    user_pcb->cr3 = (uint64_t)create_user_address_space();
    user_pcb->rip = (uint64_t)&test_user_thread;
    offset = user_pcb->rip & 0xfff;

    user_pcb->state = TASK_READY;
    user_pcb->next_proc = first_process;
    vma = (mm_struct_t *)kmalloc(1000);
    user_pcb->mm = vma;

    user_pcb->rsp = user_pcb->init_kernel = ((uint64_t)user_pcb + PAGE_SIZE);

    set_user_space(user_pcb, offset);

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

    pcb_t *user_process = create_user_process();
    switch_to_ring3((pcb_t *)user_process);
    while(1);
}


/* 
 * Test function to check new kernel thread
 */
void test_function() {
    while(1) {
	kprintf("I am in thread 1 after context switch\n");
	switchBack(next_process, first_process);
    }
}

/* Dummy function to test user thread */
void test_user_thread() {
//    kprintf("Random\n");
    int a = 10;
    a = a * 12;
    while(1);
}


void scheduler() {
//    first_process = curr_process;
//    curr_process = next_process;
    rsp_addr = (uint64_t *)(next_process->init_kernel - 128);
    switchTask(curr_process, next_process);
}


/*
 * Process to switch from 1st to 2nd process
 */
void switchTask(pcb_t *current, pcb_t *next) {

//    __asm__ volatile ("cli"); 
    __asm__ volatile ("movq %%rsp, %0;" : "=r"((current->init_kernel)));
    current->init_kernel += 8;
    __asm__ volatile ("movq %0, %%rsp;" :: "r"((next->init_kernel)));
    __asm__ volatile ("movq %0, %%rax"::"r" (next->rip));
    __asm__ volatile ("pushq %%rax;" :: );
    __asm__ volatile ("movq %%rsp, (%0)":"=r"(rsp_addr));

    next->init_kernel = (uint64_t)rsp_addr;
    set_tss_rsp((void *)rsp_addr);

    __asm__ volatile ("ret");

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


void switch_to_ring3(pcb_t *proc) {
    proc->state = TASK_RUNNING;

/*        volatile unsigned int a = 0x2B;

        _sm__ volatile ("pushq %%rax" ::);
_asm volatile("movq %0,%%rax;\n\t"
            "ltr (%%rax);"::"r"(&a)); */

    set_tss_rsp((void *) proc->init_kernel);

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
//    __asm__ volatile ("popq %%rax" ::);
//    __asm__ volatile ("orq $0x200, %%rax;" ::);
//    __asm__ volatile ("pushq %%rax" ::);
    __asm__ volatile ("pushq $0x1B" ::);
    __asm__ volatile ("pushq %0;" :: "r"(proc->rip));
    __asm__ volatile ("movq $0x0, %%rdi" ::);
    __asm__ volatile ("movq $0x0, %%rsi" ::);
    __asm__ volatile ("iretq" ::);
//    __asm__ volatile ("sti" ::);


}

void set_user_space(pcb_t *user_process, uint64_t offset) {
    uint64_t* user_page = (uint64_t *)user_process->rip;
    uint64_t* user_virtual_address;

    user_page = (uint64_t *)(0x000000000FFFF000UL & (uint64_t)user_page);
    user_virtual_address = (uint64_t *)((uint64_t)0x88888fff80000000UL | (uint64_t)user_page);

    map_phys_to_user_virt_addr((uint64_t)user_virtual_address, (uint64_t)user_page, (uint64_t *)user_process->cr3);
    user_process->rip = (uint64_t)user_virtual_address | offset;

}
