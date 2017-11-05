#include <sys/defs.h>
#include <string.h>
#include <sys/process.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <sys/kprintf.h>

uint64_t processID = 0;
pcb_t *first_process;
pcb_t *curr_process;

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
    int size;
    pcb_t *k_pcb = (pcb_t *)kmalloc((int)6000);
    
    k_pcb->pid = get_next_processID();
    k_pcb->ppid = 0;
//    strcpy(k_pcb->p_name, "main thread");
    k_pcb->cr3 = (uint64_t)get_CR3_address();
    k_pcb->state = TASK_RUNNING;
    k_pcb->next_proc = k_pcb;
    k_pcb->mm = NULL;

    first_process = (pcb_t *)k_pcb;
    curr_process = (pcb_t *)k_pcb;
    
    size = sizeof(pcb_t);
    kprintf("size: %d\n",size);
    k_pcb->rsp = k_pcb->init_kernel = ((uint64_t)k_pcb + size - 16);

    return;
}

