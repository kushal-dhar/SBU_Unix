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
#include <sys/string.h>
#include <sys/paging_helper.h>
#include <sys/process.h>
#include <sys/elf64.h>
#include <sys/syscalls.h>
#include <sys/vma.h>
#include <sys/picassem.h>

extern uint64_t *pml4;
uint64_t *old_rsp;
uint64_t *rsp_addr;
uint64_t processID = 0;
uint64_t rsp_pointer = 0;
//uint64_t *user_virtual_address = (uint64_t*)0x88888fff80000000UL;
pcb_t *first_process;
pcb_t *curr_process;
pcb_t *next_process;
pcb_t *zombie_process   = NULL;
pcb_t *sleep_process    = NULL;
pcb_t *orphan_process   = NULL;
pcb_t *stopped_process  = NULL;
pcb_t *forked_process   = NULL;


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
    
    strcpy("kernel", k_pcb->p_name);
    k_pcb->pid = get_next_processID();
    k_pcb->ppid = 1;
    k_pcb->cr3 = (uint64_t)get_CR3_address();
    k_pcb->state = TASK_RUNNING;
    k_pcb->next_proc = next_process;
    k_pcb->mm = NULL;
    //k_pcb->rip = (uint64_t)&initial_ret_function;

    first_process = (pcb_t *)k_pcb;
    curr_process = (pcb_t *)k_pcb;
    curr_process->next_proc = NULL;
    
    k_pcb->rsp = k_pcb->init_kernel = ((uint64_t)k_pcb + PAGE_SIZE);
    k_pcb->rsp -= 8;
    *(uint64_t *)(k_pcb->rsp) = (uint64_t)&initial_ret_function;

#if 0
    create_new_thread();

    /* Load the current PCB and kernel stack and disable interrupt */
    __asm__ volatile ("cli");
    __asm__ volatile ("movq %0, %%rsp;" :: "a"((k_pcb->init_kernel)));

    scheduler();
#endif

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
    child_pcb->cr3 = (uint64_t)get_CR3_address();
    child_pcb->state = TASK_READY;
    child_pcb->next_proc = curr_process;
    child_pcb->mm = NULL;
    child_pcb->rip = (uint64_t)&test_function;

    child_pcb->rsp = child_pcb->init_kernel = ((uint64_t)child_pcb + PAGE_SIZE);
    child_pcb->rsp  -= 8;
    *(uint64_t *)(child_pcb->rsp) = (uint64_t)&test_function;
    child_pcb->regs.rax = child_pcb->regs.rbx = child_pcb->regs.rdi = child_pcb->regs.rsi = child_pcb->regs.rbp = 0;
    curr_process->next_proc = child_pcb;
    child_pcb->next_proc = curr_process;
    curr_process = child_pcb;
    
    return;
}


/* 
 * Create PCB for user process and loads the same process
 */
pcb_t* create_user_process(char *filename) {
    pcb_t *user_pcb = (pcb_t *)kmalloc((int)6000);
//    uint64_t     offset       = 0;
//    uint64_t     pAddr;
//    uint64_t     vAddr;
    uint64_t    *user_virt;
    uint64_t    *user_stack;
    uint64_t    *file_pt;
    int          ret_val;
    mm_struct_t *mm;

    user_pcb->pid = get_next_processID();
    user_pcb->ppid = 1;
    user_pcb->cr3 = (uint64_t)create_user_address_space();
    strcpy(filename, user_pcb->p_name);
    strcpy(ROOT, user_pcb->curr_dir);
    strcpy(ROOT, user_pcb->temp_curr_dir);
    strcpy("PATH", user_pcb->echo_var_key[0]);
    strcpy("rootfs/bin/", user_pcb->echo_var[0]);
    user_pcb->echo_count =1;
//    user_virt = (uint64_t *)((uint64_t)USER_VIRT_ADDR | user_pcb->cr3);

    user_stack = (uint64_t *)allocate_virt_page();
    user_virt = (uint64_t *)((uint64_t)STACK_END - PAGE_SIZE);
    map_phys_to_virt_addr((uint64_t)user_virt, (uint64_t)user_stack);
    map_phys_to_user_virt_addr((uint64_t)user_virt, (uint64_t)user_stack, (uint64_t *)user_pcb->cr3);
    set_CR3((uint64_t)user_pcb->cr3);
    memset((void*)user_virt, 0, (uint32_t)PAGE_SIZE);
    set_CR3((uint64_t)first_process->cr3);

    mm = (mm_struct_t *)kmalloc(PAGE_SIZE);
    user_pcb->mm = (mm_struct_t *)mm;
    set_CR3((uint64_t)user_pcb->cr3);

    file_pt = (uint64_t *)is_file_exist(filename);
    ret_val = load_binaries(user_pcb, (uint64_t *)file_pt);
    if (ret_val == -1) {
	kprintf("Unable to load binaries\n");
	return NULL;
    }

    set_CR3(first_process->cr3);

    user_pcb->state = TASK_READY;
    user_pcb->next_proc = first_process;

    user_pcb->rsp = user_pcb->user_stack = ((uint64_t)user_virt + PAGE_SIZE - 8);
    user_pcb->init_kernel = (uint64_t)(user_pcb + PAGE_SIZE - 8);


    curr_process->next_proc = user_pcb;
    user_pcb->next_proc = curr_process;
    curr_process = user_pcb; 

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

#if 0
    int  i = 0;
    while (i < 4) {
        kprintf("I am in kernel thread again\n");
        switchTask(first_process, next_process);
        i ++;
    }
#endif 

    init_syscalls();
    
    pcb_t *user_process = create_user_process("bin/init");
    enable_page_fault();
    switch_to_ring3((pcb_t *)user_process);

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
}

/* Dummy function to test user thread */
void test_user_thread() {
    uint64_t a = 2;
    uint64_t b = 3;
    uint64_t c = 5;
    
//    kprintf("Trying to print something\n");
    __asm__ volatile("movq %0, %%rax;"::"a"(c));
    __asm__ volatile("movq %0, %%rbx;"::"a"(a));
    __asm__ volatile("movq %0, %%rcx;"::"a"(b));
    __asm__ volatile("int $0x80");
    while(1);
}


void scheduler() {
    curr_process = curr_process->next_proc;
    next_process = curr_process->next_proc;

    switchTask(curr_process, next_process);
}

void schedule_sbush() {
    pcb_t *user_process = create_user_process("bin/init");
    enable_page_fault();
//    create_user_process("bin/hello");
    switch_to_ring3((pcb_t *)user_process);
}


/*
 * Process to switch from 1st to 2nd process
 */
void switchTask(pcb_t *current, pcb_t *next) {

    __asm__ volatile("movq %%rdi, %0;" : "=a"(current->regs.rdi));
    __asm__ volatile("movq %%rsi, %0;" : "=a"(current->regs.rsi));
    __asm__ volatile("movq %%rax, %0;" : "=a"(current->regs.rax));
    __asm__ volatile("movq %%rbx, %0;" : "=a"(current->regs.rbx));
    __asm__ volatile("movq %%rbp, %0;" : "=a"(current->regs.rbp));

    __asm__ volatile ("movq  %%rsp, %0;":"=a"(current->rsp));
    __asm__ volatile ("movq %0, %%rsp;"::"a"(next->rsp));

    __asm__ volatile ("movq %0, %%rdi;" :: "a"(next->regs.rdi));
    __asm__ volatile ("movq %0, %%rax;" :: "a"(next->regs.rax));
    __asm__ volatile ("movq %0, %%rbx;" :: "a"(next->regs.rbx));
    __asm__ volatile ("movq %0, %%rbp;" :: "a"(next->regs.rbp));
    __asm__ volatile ("movq %0, %%rsi;" :: "a"(next->regs.rsi));

}
/*
 * Process to switch between user process after yield
 */
void user_switchTask(pcb_t *current, pcb_t *next) {

    set_tss_rsp((void *)next->init_kernel);

    __asm__ volatile ("movq %0, %%cr3;" :: "a"(next->cr3));
    __asm__ volatile ("movq  %%rsp, %0;":"=a"(current->kern_rsp));
    __asm__ volatile ("movq %0, %%rsp;"::"a"(next->kern_rsp));

    __asm__ volatile ("addq $8, %rsp;");
    __asm__ volatile("ret");

}


/*
 * Process to switch back from 2nd to 1st process
 */
void switchBack(pcb_t *current, pcb_t *next) {
    
    __asm__ volatile ("movq %%rsp, %0;" : "=a"((current->init_kernel)));
    __asm__ volatile ("movq %0, %%rsp;" :: "a"((next->init_kernel)));

    next->init_kernel = (uint64_t)next->init_kernel;
    set_tss_rsp((void *)next->init_kernel);

    __asm__ volatile ("ret");

}

/*
 * Stub to switch back from user to kernel process
 */
void user_switchBack(pcb_t *next) {
    next->state = TASK_RUNNING;

    set_tss_rsp((void *)next->init_kernel);

    __asm__ volatile ("movq %0, %%rsp;" :: "a"((next->kern_rsp)));
    __asm__ volatile ("movq %0, %%cr3;" :: "a"(next->cr3));


    __asm__ volatile ("ret");

}


/*
 * Stub to switch ring 3
 */
void switch_to_ring3(pcb_t *proc) {
    curr_process = curr_process->next_proc;
    next_process = curr_process->next_proc;
    kprintf("process: %d",curr_process);

    proc->state = TASK_RUNNING;
    first_process->state = TASK_READY;
    curr_process = proc;
    

    set_tss_rsp((void *)proc->init_kernel);

    __asm__ volatile ("cli" ::);
    __asm__ volatile ("movq %%rsp, %0;" : "=a"((first_process->kern_rsp)));
    __asm__ volatile ("movq %0, %%cr3;" :: "a"(proc->cr3));
    __asm__ volatile ("mov $0x23, %%ax" ::);
    __asm__ volatile ("mov %%ax, %%ds;" ::);
    __asm__ volatile ("mov %%ax, %%es;" ::);
    __asm__ volatile ("mov %%ax, %%fs;" ::);
    __asm__ volatile ("mov %%ax, %%gs;" ::);
    __asm__ volatile ("movq %0, %%rax;"::"a"(proc->rsp));
    __asm__ volatile ("pushq $0x23" ::);
    __asm__ volatile ("pushq %%rax" ::);
    __asm__ volatile ("pushfq" ::);
    __asm__ volatile ("popq %%rax" ::);
    __asm__ volatile ("orq $0x200, %%rax;" ::);
    __asm__ volatile ("pushq %%rax" ::); 
    __asm__ volatile ("pushq $0x2B" ::);
    __asm__ volatile ("pushq %0;" :: "a"(proc->rip));
    __asm__ volatile ("movq $0x0, %%rdi" ::);
    __asm__ volatile ("movq $0x0, %%rsi" ::);
    __asm__ volatile ("iretq" ::);

}

/*
 * Stub to switch ring 3
 */
void execve_switch_to_ring3(pcb_t *proc) {
    proc->state = TASK_RUNNING;
    curr_process = proc;

    set_tss_rsp((void *)proc->init_kernel);

    __asm__ volatile ("cli" ::);
    __asm__ volatile ("movq %0, %%cr3;" :: "a"(proc->cr3));
    __asm__ volatile ("mov $0x23, %%ax" ::);
    __asm__ volatile ("mov %%ax, %%ds;" ::);
    __asm__ volatile ("mov %%ax, %%es;" ::);
    __asm__ volatile ("mov %%ax, %%fs;" ::);
    __asm__ volatile ("mov %%ax, %%gs;" ::);
    __asm__ volatile ("movq %0, %%rax;"::"a"(proc->rsp));
    __asm__ volatile ("pushq $0x23" ::);
    __asm__ volatile ("pushq %%rax" ::);
    __asm__ volatile ("pushfq" ::);
    __asm__ volatile ("popq %%rax" ::);
    __asm__ volatile ("orq $0x200, %%rax;" ::);
    __asm__ volatile ("pushq %%rax" ::);
    __asm__ volatile ("pushq $0x2B" ::);
    __asm__ volatile ("pushq %0;" :: "a"(proc->rip));
    __asm__ volatile ("movq $0x0, %%rdi" ::);
    __asm__ volatile ("movq $0x0, %%rsi" ::);
    __asm__ volatile ("iretq" ::);

}


void set_user_space(pcb_t *user_process, uint64_t offset) {
    uint64_t* user_page = (uint64_t *)user_process->rip;
    uint64_t* user_virtual_addr;

    user_page = (uint64_t *)(0x000000000FFFF000UL & (uint64_t)user_page);
    user_virtual_addr = (uint64_t *)((uint64_t)USER_VIRT_ADDR| (uint64_t)user_page);
    
    map_phys_to_virt_addr((uint64_t)user_virtual_addr, (uint64_t)user_page);
    map_phys_to_user_virt_addr((uint64_t)user_virtual_addr, (uint64_t)user_page, (uint64_t *)user_process->cr3);
    user_process->rip = (uint64_t)user_virtual_addr | offset;

}

pid_t fork_child() {
    pcb_t *c_pcb;

    /* Copy parents structure to child's structure */
    c_pcb = (pcb_t *)copy_parent_structure((pcb_t *)curr_process);

    c_pcb->next_proc = curr_process->next_proc;
    curr_process->next_proc = c_pcb;
    return c_pcb->pid;
}


pcb_t* copy_parent_structure(pcb_t *parent_proc) {
    volatile vma_t     *parent_vma     = NULL;
    vma_t     *child_vma      = NULL;
    uint64_t   count          = 0;
    pcb_t     *child_pcb      = (pcb_t *)kmalloc(6000);

    child_pcb->pid = get_next_processID();
    child_pcb->ppid = curr_process->pid;
    strcpy(curr_process->p_name, child_pcb->p_name);
    strcpy(curr_process->curr_dir, child_pcb->curr_dir);
    strcpy(curr_process->temp_curr_dir, child_pcb->temp_curr_dir);
    child_pcb->echo_count= curr_process-> echo_count;
    for (int i =0; i < 5 ;i++){
      strcpy(curr_process->echo_var[i], child_pcb->echo_var[i]);
      strcpy(curr_process->echo_var_key[i], child_pcb->echo_var_key[i]);
    }   
    child_pcb->next_proc = NULL;
    child_pcb->mm = NULL;
    
    curr_process->n_child += 1;
    child_pcb->n_child = 0;

    child_pcb->cr3 = (uint64_t)create_user_address_space();
    __asm__ volatile("mov %%cr3, %0" : "=a" (curr_process->cr3));
    copy_parent_tables((uint64_t *)child_pcb->cr3);

    child_pcb->init_kernel = (uint64_t)(child_pcb + PAGE_SIZE - 8);

    child_pcb->mm = (mm_struct_t *)kmalloc(PAGE_SIZE);
    memcpy(parent_proc->mm, child_pcb->mm, sizeof(mm_struct_t));

    parent_vma = parent_proc->mm->vma;
    
   while (parent_vma != NULL) {
        if (child_vma == NULL) {
            count = 1;
        }
        child_vma = (vma_t *)kmalloc(PAGE_SIZE);
        memcpy(parent_vma, child_vma, sizeof(vma_t));
        child_vma->vm_next = NULL;
        child_vma->vm_mm = child_pcb->mm;

        if (parent_vma->file != NULL) {
            child_vma->file = (tarfile_t *)kmalloc(PAGE_SIZE);
            memcpy(parent_vma->file, child_vma->file, sizeof(tarfile_t));
        }
        if (count == 1) {
            child_pcb->mm->vma = child_vma;
            count = 0;
        }
        else {
            add_mmstruct(child_pcb->mm, child_vma);
        }
        parent_vma = parent_vma->vm_next;
    }
//    set_CR3((uint64_t)child_pcb->cr3);
    memcpy((void *)parent_proc->init_kernel + 8 - PAGE_SIZE, (void *)child_pcb->init_kernel + 8 - PAGE_SIZE, PAGE_SIZE);
    child_pcb->user_stack = (uint64_t)parent_proc->user_stack;
    child_pcb->rsp = (uint64_t)(child_pcb->init_kernel - (parent_proc->init_kernel - rsp_pointer));
    
  
    set_CR3(curr_process->cr3);
                                                                                            
    return child_pcb;
}

void wait(uint64_t pid) {
    uint64_t     val         = 0;
    pcb_t       *prev_task;

    prev_task = curr_process;
    curr_process->kern_rsp=rsp_pointer;
    curr_process->state = TASK_READY;
    curr_process = curr_process->next_proc;
    next_process = curr_process->next_proc;
    curr_process->state = TASK_RUNNING;

    if (curr_process->pid == pid) {
        set_tss_rsp((void *)curr_process->init_kernel);

        __asm__ volatile("movq %%rdi, %0;" : "=a"(prev_task->regs.rdi));
        __asm__ volatile("movq %%rsi, %0;" : "=a"(prev_task->regs.rsi));
    	__asm__ volatile("movq %%rax, %0;" : "=a"(prev_task->regs.rax));
    	__asm__ volatile("movq %%rbx, %0;" : "=a"(prev_task->regs.rbx));
    	__asm__ volatile("movq %%rbp, %0;" : "=a"(prev_task->regs.rbp));
        
     	__asm__ volatile ("movq %%rsp, %0;" : "=a"(prev_task->kern_rsp));
//        __asm__ volatile ("movq %%rip, %0;" : "=a"((prev_task->rip)));      

        __asm__ volatile ("movq %0, %%cr3;" :: "a"(curr_process->cr3));
        __asm__ volatile ("movq %0, %%rsp;" :: "a"(curr_process->rsp));
        __asm__ volatile ("movq %0, %%rax;" :: "a"(val));
        __asm__ volatile ("iretq");
    }
    return;
}

uint64_t get_pid(){
  return (uint64_t) curr_process->pid;
}


void execve(char *filename, char *argv) {
    pcb_t       *user_pcb = (pcb_t *)kmalloc((int)6000);
    pcb_t       *pointer;
    uint64_t    *user_virt;
    uint64_t    *user_stack;
    uint64_t    *file_pt;
    char         file[100];
    char         kernel_args[5][64];
    int          ret_val;
    int          i           = 0;
    int          argc        = 0;
    int          j           = 0;
    int	         k           = 0;
    mm_struct_t *mm;

    while (*filename != '\0') {
        file[i++] = *filename;
        filename++;
    }
    file[i] = '\0';

    i = 0;
    while(argv[k] != '\0') {
	j = 0;
	while (argv[k] != ' ' && argv[k] != '\0' && argv[k] != '\n') {
	    kernel_args[i][j++] = (char)argv[k++]; 
	}
	kernel_args[i][j] = '\0';
	i++;
	argc++;
	k++;
    }
  
    argc = i;

    strcpy(file, user_pcb->p_name);
    user_pcb->pid = curr_process->pid;
    user_pcb->ppid = curr_process->ppid;
    user_pcb->state = TASK_RUNNING;
    user_pcb->n_child = 0;
    strcpy(curr_process->curr_dir, user_pcb->curr_dir);
    strcpy(curr_process->temp_curr_dir, user_pcb->temp_curr_dir);
    user_pcb->echo_count = curr_process-> echo_count;;
    for (int i =0; i <5 ;i++){
     strcpy(curr_process->echo_var[i], user_pcb->echo_var[i]);
     strcpy(curr_process->echo_var_key[i], user_pcb->echo_var_key[i]);
     }
    user_pcb->cr3 = (uint64_t)create_user_address_space();

    user_stack = (uint64_t *)allocate_virt_page();
    user_virt = (uint64_t *)((uint64_t)STACK_END - PAGE_SIZE);
    map_phys_to_virt_addr((uint64_t)user_virt, (uint64_t)user_stack);
    map_phys_to_user_virt_addr((uint64_t)user_virt, (uint64_t)user_stack, (uint64_t *)user_pcb->cr3);
    set_CR3((uint64_t)user_pcb->cr3);
    memset((void*)user_virt, 0, (uint32_t)PAGE_SIZE);
    set_CR3((uint64_t)first_process->cr3);

    mm = (mm_struct_t *)kmalloc(PAGE_SIZE);
    user_pcb->mm = (mm_struct_t *)mm;
    set_CR3((uint64_t)user_pcb->cr3);

    file_pt = (uint64_t *)is_file_exist(file);
    ret_val = load_binaries(user_pcb, (uint64_t *)file_pt);
    if (ret_val == -1) {
        kprintf("Unable to load binaries\n");
        return;
    }

    set_CR3(first_process->cr3);

    user_pcb->state = TASK_READY;
    user_pcb->next_proc = first_process;

    user_pcb->rsp = user_pcb->user_stack = ((uint64_t)user_virt +PAGE_SIZE - 8);
    user_pcb->init_kernel = (uint64_t)(user_pcb + PAGE_SIZE - 8);

    //user_pcb->next_proc = curr_process->next_proc;
    pointer = curr_process;
    while(pointer->next_proc != curr_process) {
	pointer = pointer->next_proc;
    }
    pointer->next_proc = user_pcb;
    user_pcb->next_proc = curr_process->next_proc;

    pointer = forked_process;
    if (pointer == NULL) {
	pointer = curr_process;
	curr_process->next_proc = NULL;
	forked_process = curr_process;
    }
    else {
	while (pointer->next_proc != NULL) {
	    pointer = pointer->next_proc;
	}
	pointer->next_proc = curr_process;
	curr_process->next_proc = NULL;
    } 

    set_CR3(user_pcb->cr3);

    /*Set user stack to contain argc and argv values */
    i = sizeof(kernel_args);  
    char *ptr = (char *)(user_pcb->user_stack - sizeof(kernel_args));
    memcpy((void *)kernel_args, (void *)ptr, sizeof(kernel_args));
    ptr = ptr - 8;
    *((int *)ptr) = argc;
    user_pcb->rsp = user_pcb->user_stack - sizeof(kernel_args) - 8;

    execve_switch_to_ring3(user_pcb);
}

void print_allPID(){
    kprintf("PID\tPPID\tProcess\tStatus\n");
    pcb_t *proc=  first_process;
    while(proc->next_proc != first_process){
        kprintf("%d\t\t%d\t%s",proc->pid,proc->ppid,proc->p_name);
	if (proc->state == TASK_READY) {
	    kprintf("\t%s\n","Ready");
	}
	else if (proc->state == TASK_RUNNING) {
	    kprintf("\t%s\n","Running");
	}
	proc = proc->next_proc;
	if (proc->next_proc == first_process) {
            kprintf("%d\t\t%d\t%s",proc->pid,proc->ppid,proc->p_name);
    	    if (proc->state == TASK_READY) {
                kprintf("\t%s\n","Ready");
            }
            else if (proc->state == TASK_RUNNING) {
                kprintf("\t%s\n","Running");
            }
	}
	
    } 
}


/* 
 * Handles exit syscall
 */
void sys_exit() {
    pcb_t     *prev_task;
    pcb_t     *iterator;
    pcb_t     *stopped_list;
    uint64_t   new_page;

    prev_task = curr_process;
    curr_process->state = TASK_STOPPED;

    while (prev_task->next_proc->pid != curr_process->pid) {
    	prev_task = prev_task->next_proc;
    }
    prev_task->next_proc = first_process;

    stopped_list = stopped_process;
    while(stopped_list != NULL) {
	iterator = stopped_list->next_proc;
        free_vma(stopped_list->mm, stopped_list->cr3);
        free_page(stopped_list->user_stack, stopped_list->cr3);
//        delete_pagetable(stopped_list->cr3);
//        free_page((uint64_t)stopped_list, curr_process->cr3);
	stopped_list = iterator;
    }
    stopped_process = NULL;
    stopped_process = curr_process;
    curr_process->next_proc = NULL;

    iterator = forked_process;
    while(iterator != NULL) {
	stopped_list = iterator;
	iterator = iterator->next_proc;
        free_vma(stopped_list->mm, stopped_list->cr3);
        free_page(stopped_list->user_stack, stopped_list->cr3);
//        delete_pagetable(stopped_list->cr3);
//        free_kernel_page((uint64_t)stopped_list);
    }
    forked_process = NULL;
    
    set_CR3(curr_process->cr3);
    new_page = (uint64_t)allocate_virt_page();
    if (new_page != 0x0) {
        kprintf("%s","");
    }
    
    
    curr_process = prev_task;
    if (curr_process->pid == 1) {
	prev_task->init_kernel = prev_task->kern_rsp = (uint64_t)prev_task + PAGE_SIZE - 8;
	memset((void*)prev_task, 0, (uint32_t)PAGE_SIZE);
	kprintf("\n\n\n******************Terminal terminated******************\n\n\n");
	clear_console();
        *(uint64_t *)(prev_task->kern_rsp) = (uint64_t)&schedule_sbush;       
	user_switchBack(prev_task);
        
    }

    user_switchTask(prev_task->next_proc, curr_process);
}

/*
 * Handle kill syscall 
 */
void kill(uint64_t pid) {
    pcb_t     *iterator;
    pcb_t     *pointer;
    pcb_t     *prev_task      = NULL;
    pcb_t     *stopped_list;
    uint64_t   deleted        = 0;

    if (pid == curr_process->pid) {
	sys_exit();
    }
    else {
	iterator = first_process;
 	while(iterator->pid != pid && iterator->next_proc != first_process) {
	    prev_task = iterator;
	    iterator = iterator->next_proc;
	}
	iterator = prev_task;
 	if (iterator->next_proc->pid == pid) {
            /* Remove all child process from the ready queue */
            pointer = iterator->next_proc->next_proc;
            while(pointer->next_proc != iterator->next_proc) {
	        if (pointer->ppid == pid) {
		    pointer->ppid = 1;
		}
		pointer = pointer->next_proc;
	    }

	    stopped_list = stopped_process;
	    if (stopped_list == NULL) {
		stopped_process = iterator->next_proc;
		iterator->next_proc = stopped_process->next_proc;
		stopped_process->next_proc = NULL;
		kprintf("Killed %d\n",pid);
		deleted = 1;
	    }
	    else {
		prev_task = stopped_list;
   	        while(stopped_list != NULL) {
		    prev_task = stopped_list;
		    stopped_list = stopped_list->next_proc;
		}
		prev_task->next_proc = iterator->next_proc;
		iterator->next_proc = prev_task->next_proc->next_proc;
		prev_task->next_proc->next_proc = NULL;
		kprintf("Killed %d\n",pid);
		deleted = 1;
	    }
         
         for(int i =0; i <200;i++)
           kprintf("%s","******************************************************************************* ");
         // clear_console();
         
	}
	
	/* Process to be deleted is not in ready process */
	if (deleted == 0) {
	    kprintf("kill: (%d) - No such process\n",pid);
	}
    }
    return;
    
}

/** Set ECHO environment" **/
void setEnv(char *s){
  char buf[100];
  strcpy(s,buf);
  char *temp = buf;
  char *key = temp;
  char *value;
  while(*temp != '=') temp++;
  *temp='\0';
  temp++;
  value = temp;
  for(int i =0; i <5; i++)
  {
      if (strcmp(curr_process->echo_var_key[i],key) ==0){
          strcpy(value,curr_process->echo_var[i] );
          return;   
      }
  }   
  int index = curr_process->echo_count;
  strcpy(key, curr_process->echo_var_key[index]);
  strcpy(value, curr_process->echo_var[index]);
  curr_process->echo_count = index+1;
  if ((index+1) == 5){
    curr_process->echo_count =0;
  }
}
void printEnv(char *s){
 for(int i =0; i <5; i++){
     if(strcmp(curr_process->echo_var_key[i],s)==0){
        kprintf(curr_process->echo_var[i]); 
        kprintf("\n");
        return;
     }	
 }
  kprintf("%s\n","sbush: No such environment variable");
  #if 0
 for(int i =0; i <5; i++){
     char buf [100];
     strcpy(curr_process->echo_var[i], buf);
     char * temp = buf;
//     int len = strlen(buf);
     while(*temp != '\0' && *temp != '=') temp++;
     char * first = temp;
     if (*temp == '=')
          temp++;
         *first='\0';
          first = buf;
        if (strcmp(first,s) == 0){
     //     *(temp+len-1) = '\0';
          kprintf("\n%s",temp);
          return;
        }
     }
 kprintf("%s","sbush: No such environment variable");
#endif
}
