#include <sys/defs.h>
#include <string.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <sys/kprintf.h>
#include <sys/pic.h>
#include <sys/idt.h>
#include <sys/process.h>
#include <sys/elf64.h>

uint64_t *pml4;	
uint64_t *pdpt;
uint64_t *pd;
uint64_t *pt;

extern char kernmem;
uint64_t *free_map;
uint64_t cr3;
uint64_t physfree_start, physfree_end;
uint64_t virt_addr = (uint64_t)KERNEL_BASE;
//uint64_t user_addr = (uint64_t)0x88888fff80000000UL;
extern pcb_t *first_process;
extern pcb_t *curr_process;
/* These three  are for mmap function **/
uint64_t virtual_mmap = (uint64_t) 0x88888000000UL;
//vma_t *vma_mmap;
#define HEAP    2
/**********/


/*
 * Initializing the free pages and pagelist entries
 */
void init_paging(uint64_t physbase, uint64_t physfree, uint64_t base, uint64_t length) {
    int        start       = 0;
    int        end         = 0;
    uint64_t   address     = 0;
    
    free_map = (uint64_t *)physfree;
    free_map = memset((void*)free_map, 0, (uint32_t)256 * sizeof(uint64_t));
   
    start = 0;
    end = (base+length) / PAGE_SIZE;
    physfree_start = physfree;
    physfree_start += (uint64_t)(256 * 64/8);
    //physfree_start += (uint64_t)8192;
    physfree_end = base + length;
    kprintf("physfree %d\n physend: %d\n",physfree_start, physfree_end);

    while (start < end) {
	address = start * PAGE_SIZE;
 	if (address < physfree_start) {
	/* For address lower than 1MB, dont set free page-table */
	    start ++;
	    continue;
	}
	else {
    	    set_bit(free_map, start);
  	    start++;
	}
    }
}

/*
 * Get first free page table index
 */
int get_firstpage() {
    int            i            = 0;
    int            j            = 0;

    for (i = 0; i < 256; i++) {
	for (j = 0; j < 64; j++) {
	    if (free_map[i] & (1ull << j)) {
		return (i*64 + j);
	    }
      	}
    }
    return -1;
}

/*
 * Take the address of first page and return that after allocating
 */
uint64_t* allocate_page() {
    uint64_t      page_addr         = 0;
    uint64_t      page_index        = get_firstpage();

    unset_bit(free_map, page_index);
    page_addr = page_index * PAGE_SIZE;
    memset((void*)page_addr, 0, (uint32_t)PAGE_SIZE);

    return (uint64_t *)page_addr;
}

/*
 * Take the address of first page and return that after allocating
 * Dont call memset, as memset wont work on physicall address after enabling page table.
 */
uint64_t* allocate_virt_page() {
    uint64_t      page_addr         = 0;
    uint64_t      page_index        = get_firstpage();

    unset_bit(free_map, page_index);
    page_addr = page_index * PAGE_SIZE;

    return (uint64_t *)page_addr;
}



void pdpt_allocate(uint64_t *pml4, uint64_t pml4_index) {
    pdpt = (uint64_t *)allocate_page();
    uint64_t pdpt_entry = (uint64_t)pdpt;
    pdpt_entry |= (PT_PR | PT_WR );

    pml4[pml4_index] = pdpt_entry;
    return ;
}

void pd_allocate(uint64_t* pdpt, uint64_t pdpt_index) {
    pd = (uint64_t *)allocate_page();
    uint64_t pd_entry = (uint64_t)pd;
    pd_entry |= (PT_PR | PT_WR );

    pdpt[pdpt_index] = pd_entry;
    return;
}

void pt_allocate(uint64_t* pd, uint64_t pd_index) {
    pt = (uint64_t *)allocate_page();
    uint64_t pt_entry = (uint64_t)pt;
    pt_entry |= (PT_PR | PT_WR );

    pd[pd_index] = pt_entry;
    return;
}

/* 
 * Setup page table for kernel memory, free_map and for video memory
 */
void setup_page_tables(uint64_t phys_base, uint64_t phys_free) {
    uint64_t  vAddress = (uint64_t)&kernmem;
    uint64_t  pml4_index = get_index(vAddress, PML4_INDEX);
    uint64_t  pdpt_index = get_index(vAddress, PDPT_INDEX);
    uint64_t  pd_index = get_index(vAddress, PD_INDEX);
    uint64_t  pt_index = get_index(vAddress, PT_INDEX);
    uint64_t  address_ptr = phys_base;

    pml4 = (uint64_t *)allocate_page();
    address_ptr = (uint64_t)pml4;
    cr3 = (uint64_t)address_ptr;
    kprintf("cr3 address now: %x\n",cr3);

    pdpt_allocate(pml4, pml4_index);
    pd_allocate(pdpt, pdpt_index);
    pt_allocate(pd, pd_index);

    /* Allocate page_table entries for kernel memory */
    for(; phys_base < physfree_start; phys_base += 0x1000, vAddress += 0x1000) {
	pt_index = get_index(vAddress, PT_INDEX);
	address_ptr = phys_base;
	address_ptr |= (PT_PR | PT_WR );
	pt[pt_index] = address_ptr;
    }

    kprintf("physfree_start: %x  \n",physfree_start);

    vAddress = (uint64_t)&kernmem;
    vAddress &= 0xFFFFF000000;
    vAddress |= VIDEO_PHYS_ADDR;
    phys_base = VIDEO_PHYS_ADDR;
    //pml4_index = get_index(vAddress, PML4_INDEX);
    //pdpt_index = get_index(vAddress, PDPT_INDEX);
    //pd_index = get_index(vAddress, PD_INDEX);
    pd_index = get_index(vAddress, PD_INDEX);
    pt_index = get_index(vAddress, PT_INDEX);
    pt_allocate(pd, pd_index);

    //pt_allocate(pd, pd_index);
    address_ptr = phys_base;
    address_ptr |= (PT_PR | PT_WR );
    pt[pt_index] = address_ptr; 

    vAddress = KERNEL_ADDR | (uint64_t)pml4;
    map_virt_to_phys_addr((uint64_t)vAddress, (uint64_t)pml4);
    vAddress = KERNEL_ADDR | (uint64_t)pdpt;
    map_virt_to_phys_addr((uint64_t)vAddress, (uint64_t)pdpt);
    vAddress = KERNEL_ADDR | (uint64_t)pd;
    map_virt_to_phys_addr((uint64_t)vAddress, (uint64_t)pd);
    vAddress = KERNEL_ADDR | (uint64_t)pt;
    map_virt_to_phys_addr((uint64_t)vAddress, (uint64_t)pt);
    
    return;
}

void map_virt_to_phys_addr(uint64_t vAddress, uint64_t phys) {
    uint64_t  pml4_index = get_index(vAddress, PML4_INDEX);
    uint64_t  pdpt_index = get_index(vAddress, PDPT_INDEX);
    uint64_t  pd_index = get_index(vAddress, PD_INDEX);
    uint64_t  pt_index = get_index(vAddress, PT_INDEX);
    
    uint64_t pml4_addr = pml4[pml4_index];
    uint64_t pdpt_addr;
    uint64_t pd_addr;
    uint64_t pt_addr;

    if (pml4_addr & PT_PR) {
	pdpt = (uint64_t *)remove_flag(pml4_addr);
    }
    else {
    /* If pdpt entry is not allocated yet, allocate a new entry */
	pdpt = (uint64_t *)allocate_page();
	pml4[pml4_index] = (uint64_t)pdpt | (PT_PR | PT_WR );
    }
    
    pdpt_addr = pdpt[pdpt_index];
    if (pdpt_addr & PT_PR) {
	pd = (uint64_t *)remove_flag(pdpt_addr);
    }
    else {
    /* If page descriptor entry is not allocated yet, allocate a new entry */
	pd = (uint64_t *)allocate_page();
	pdpt[pdpt_index] = (uint64_t)pd | (PT_PR | PT_WR );
    }
    pd_addr = pd[pd_index];
    if (pd_addr & PT_PR) {
        pt = (uint64_t *)remove_flag(pd_addr);
    } 
    else {
    /* If page table entry is not allocated yet, allocate a new entry */
	pt = (uint64_t *)allocate_page();
	pd[pd_index]=(uint64_t)pt | (PT_PR | PT_WR );
    }
    
    pt_addr = phys;
    pt_addr |= (PT_PR | PT_WR );
    pt[pt_index] = pt_addr;

    return;
}

/*
 * Map physical address to the corresponding virtual address provided 
 * Convert all physical addr stored in page tables to virtual addr before accessing
 */
void map_phys_to_virt_addr(uint64_t vAddress, uint64_t phys) {
    uint64_t  pml4_index = (vAddress >> PML4_INDEX) & 0x1FF;
    uint64_t  pdpt_index = (vAddress >> PDPT_INDEX) & 0x1FF;;
    uint64_t  pd_index = (vAddress >> PD_INDEX) & 0x1FF;;
    uint64_t  pt_index = (vAddress >> PT_INDEX) & 0x1FF;;


    __asm__ volatile("mov %%cr3, %0" : "=a" (pml4));    
//    pml4 = (uint64_t *)get_CR3_address();
    pml4 = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)pml4);
    uint64_t pml4_addr = pml4[pml4_index];
    uint64_t pdpt_addr;
    uint64_t pd_addr;
    uint64_t pt_addr;
    uint64_t *virt_page;

    if (pml4_addr & PT_PR) {
        pml4_addr = (uint64_t )remove_flag(pml4_addr);
    	pdpt = (uint64_t *)pml4_addr;
        pdpt = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)pdpt);
    }
    else {
    /* If pdpt entry is not allocated yet, allocate a new entry */
        pdpt = (uint64_t *)allocate_virt_page();
        virt_page = (uint64_t *)((uint64_t)KERNEL_BASE | (uint64_t)pdpt);
        memset((void*)virt_page, 0, (uint32_t)PAGE_SIZE);
        pml4[pml4_index] = (uint64_t)pdpt | (PT_PR | PT_WR );
	pdpt = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)pdpt);
    }

    pdpt_addr = pdpt[pdpt_index];
    if (pdpt_addr & PT_PR) {
        pd = (uint64_t *)remove_flag(pdpt_addr);
	pd = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)pd);
    }
    else {
    /* If page descriptor entry is not allocated yet, allocate a new entry */
        pd = (uint64_t *)allocate_virt_page();
	virt_page = (uint64_t *)((uint64_t)KERNEL_BASE | (uint64_t)pd);
        memset((void*)virt_page, 0, (uint32_t)PAGE_SIZE);
        pdpt[pdpt_index] = (uint64_t)pd | (PT_PR | PT_WR );
	pd = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t )pd);
    }
    pd_addr = pd[pd_index];
    if (pd_addr & PT_PR) {
        pt = (uint64_t *)remove_flag(pd_addr);
	pt = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)pt);
    }
    else {
    /* If page table entry is not allocated yet, allocate a new entry */
        pt = (uint64_t *)allocate_virt_page();
        virt_page = (uint64_t *)((uint64_t)KERNEL_BASE | (uint64_t)pt);
        memset((void*)virt_page, 0, (uint32_t)PAGE_SIZE);
        pd[pd_index]=(uint64_t)pt | (PT_PR | PT_WR );
	pt =  (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t )pt);
    }

    pt_addr = phys;
    pt_addr |= (PT_PR | PT_WR );
    pt[pt_index] = pt_addr;

    return;
}

void set_identity_paging() {
    uint64_t vAddr = (uint64_t)KERNEL_ADDR;
    uint64_t pAddr = (uint64_t)PHYS_ADDR;
    
    for (; pAddr < physfree_end; pAddr += 0x1000, vAddr += 0x1000) {
	map_virt_to_phys_addr(vAddr, pAddr);
    }
    free_map = (uint64_t *) (KERNEL_ADDR | (uint64_t)free_map);

    return;
}

/*
 * Allocate required number of page for size requested (in bytes) and 
 * populate their corresponding page table entries. Return the virtual Addres afterwards.
 */
uint64_t* kmalloc(int size) {
    int        num_page      = 0;
    int        i             = 0;
    uint64_t   page_addr     = 0;
    uint64_t   ret_addr      = 0;

    if (size % PAGE_SIZE == 0) {
	num_page = size / PAGE_SIZE;
    }
    else {
 	num_page = (size / PAGE_SIZE) + 1;
    }
//    ret_addr = virt_addr + 0x1000;
    
    for (i = 0; i < num_page; i++) {
	page_addr = (uint64_t )allocate_virt_page();
	virt_addr = (uint64_t)(KERNEL_BASE | page_addr);
	if (i == 0) {
	    ret_addr = virt_addr;
	}
        map_phys_to_virt_addr((uint64_t)virt_addr, (uint64_t)page_addr);
	memset((void*)virt_addr, 0, (uint32_t)PAGE_SIZE);
    }
   
    return (uint64_t *)ret_addr; 
}

uint64_t* umalloc(pcb_t *proc, int size) {
    int        num_page      = 0;
    int        i             = 0;
    uint64_t   page_addr     = 0;
//    uint64_t   ret_addr;
    uint64_t  *user_virt;

    if (size % PAGE_SIZE == 0) {
        num_page = size / PAGE_SIZE;
    }
    else {
        num_page = (size / PAGE_SIZE) + 1;
    }
    //ret_addr = user_addr + 0x1000;
    user_virt = (uint64_t *)USER_VIRT_ADDR;

    for (i = 0; i < num_page; i++) {
        //user_addr += 0x1000;
        page_addr = (uint64_t )allocate_virt_page();
        user_virt = (uint64_t *)((uint64_t)USER_VIRT_ADDR | (uint64_t)page_addr);
        map_phys_to_user_virt_addr((uint64_t)user_virt, (uint64_t)page_addr, (uint64_t *)proc->cr3);
        memset((void*)user_virt, 0, (uint32_t)PAGE_SIZE);
    }

    return (uint64_t *)user_virt;
}


uint64_t* create_user_address_space() {
    uint64_t*          pml4_addr;
/*    uint64_t*          pdpt_addr;
    uint64_t*          pd_addr;
    uint64_t*	       pt_addr; */
    uint64_t*          pml4_virt_addr; 
    uint64_t*          cr3_addr;

//    virt_addr += 0x1000;
    pml4_addr = (uint64_t *)allocate_virt_page();
    virt_addr = (uint64_t)(KERNEL_BASE | (uint64_t)pml4_addr);
    map_phys_to_virt_addr((uint64_t)virt_addr, (uint64_t)pml4_addr);
    memset((void*)virt_addr, 0, (uint32_t)PAGE_SIZE);
    __asm__ volatile("mov %%cr3, %0" : "=a" (cr3_addr));
//    cr3_addr = (uint64_t)get_CR3_address();
//    cr3_addr = (uint64_t)cr3_addr  | (PT_PR | PT_WR | PT_U);
//    cr3_addr = (uint64_t)KERNEL_ADDR | cr3_addr;

    pml4_virt_addr = (uint64_t *)virt_addr;
    cr3_addr = (uint64_t *)( KERNEL_ADDR | (uint64_t)cr3_addr);
    pml4_virt_addr[511] = (uint64_t)cr3_addr[511] ;
/*    cr3_addr = cr3_addr[511];
    pdpt_addr = (uint64_t *)( KERNEL_ADDR | (uint64_t)pml4_virt_addr[511]);
    pdpt_addr[510] = (uint64_t) cr3_addr[510] | PT_U;
    cr3_addr = cr3_addr[510];
    pd_addr = (uint64_t *)( KERNEL_ADDR | (uint64_t)pml4_virt_addr[511]);
    pdpt_addr[510] = (uint64_t) cr3_addr[510] | PT_U; */


    return pml4_addr;    
}

void map_phys_to_user_virt_addr(uint64_t vAddress, uint64_t phys, uint64_t* cr3_addr) {
    uint64_t  pml4_index = (vAddress >> PML4_INDEX) & 0x1FF;
    uint64_t  pdpt_index = (vAddress >> PDPT_INDEX) & 0x1FF;
    uint64_t  pd_index = (vAddress >> PD_INDEX) & 0x1FF;
    uint64_t  pt_index = (vAddress >> PT_INDEX) & 0x1FF;

    cr3_addr = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)cr3_addr);
    uint64_t pml4_addr =  cr3_addr[pml4_index];
    uint64_t pdpt_addr;
    uint64_t pd_addr;
//    uint64_t pt_addr;
    uint64_t* page_addr;
    uint64_t* virt_page_addr;
    uint64_t* physical;

    if (pml4_addr & PT_PR) {
        pml4_addr = (uint64_t )remove_flag(pml4_addr);
        pdpt = (uint64_t *)pml4_addr;
        pdpt = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)pdpt);
    }
    else {
    /* If pdpt entry is not allocated yet, allocate a new entry */
        pdpt = (uint64_t *)allocate_virt_page();
 	virt_page_addr = (uint64_t *)((uint64_t)KERNEL_BASE | (uint64_t)pdpt);
	memset((void *)virt_page_addr, 0, PAGE_SIZE);
//	cr3_addr = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)cr3_addr);
        cr3_addr[pml4_index] = (uint64_t)pdpt | (PT_PR | PT_WR | PT_U);
        pdpt = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)pdpt);
    }

    pdpt_addr = pdpt[pdpt_index];
    if (pdpt_addr & PT_PR) {
        pd = (uint64_t *)remove_flag(pdpt_addr);
        pd = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)pd);
    }
    else {
    /* If page descriptor entry is not allocated yet, allocate a new entry */
        pd = (uint64_t *)allocate_virt_page();
        virt_page_addr = (uint64_t *)((uint64_t)KERNEL_BASE | (uint64_t)pd);
        memset((void *)virt_page_addr, 0, PAGE_SIZE);
        pdpt[pdpt_index] = (uint64_t)pd | (PT_PR | PT_WR | PT_U);
        pd = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t )pd);
    }
    pd_addr = pd[pd_index];
    if (pd_addr & PT_PR) {
        pt = (uint64_t *)remove_flag(pd_addr);
        pt = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)pt);
    }
    else {
    /* If page table entry is not allocated yet, allocate a new entry */
        pt = (uint64_t *)allocate_virt_page();
        virt_page_addr = (uint64_t *)((uint64_t)KERNEL_BASE | (uint64_t)pt);
        memset((void *)virt_page_addr, 0, PAGE_SIZE);
        pd[pd_index]=(uint64_t)pt | (PT_PR | PT_WR | PT_U);
        pt =  (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t )pt);
    }

    page_addr = (uint64_t *)allocate_virt_page();
    virt_page_addr = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t )page_addr);
    memset((void *)virt_page_addr, 0, PAGE_SIZE);
    physical = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t )phys);
//    phys = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t )phys);
    memcpy ((void *)physical, (void *)virt_page_addr, PAGE_SIZE);
//    pt_addr = phys;
//    pt_addr |= (PT_PR | PT_WR | PT_U);
//    page_addr |= (PT_PR | PT_WR | PT_U);
    pt[pt_index] = (uint64_t)page_addr | (PT_PR | PT_WR | PT_U);

    return;
}

void enable_page_fault() {
    irq_set(14, pagefault_handler);
}

void pagefault_handler(regis reg) {
    uint64_t     fault_addr     = 0;
    uint64_t     phys_addr      = 0;
    uint64_t     new_page       = 0;
    vma_t       *vma;
    
    __asm__ volatile ("mov %%cr2, %0" : "=a" (fault_addr));

    fault_addr = (uint64_t)remove_flag(fault_addr);
    phys_addr = virt_to_phys(fault_addr);
    vma = curr_process->mm->vma;
    
//    kprintf("%d\n",fault_addr);
//    while(1);
#if 0
    if (reg != NULL) {
        err_code = reg->err_code & 0xF;
    }
    //curr = curr_process->mm->vma;

    if (err_code & (PT_PR | PT_WR)) {
#endif
    if (phys_addr != 999 && phys_addr != 0) {
        new_page = (uint64_t)allocate_virt_page();
        virt_addr = (uint64_t)(KERNEL_ADDR | new_page);
        memcpy((void *)fault_addr, (void *)virt_addr, PAGE_SIZE);
        map_phys_to_user_virt_addr((uint64_t)fault_addr, (uint64_t)new_page, (uint64_t *)curr_process->cr3);
    }
    else if (vma != NULL) {
        while(vma != NULL) {
    	    if (vma->vm_start <= fault_addr && fault_addr <= vma->vm_end) {
	        if (vma->type == HEAP || vma->type == STACK || vma->type == DATA) {
		    new_page = (uint64_t)allocate_virt_page();
		    virt_addr = (uint64_t)(KERNEL_ADDR | new_page);
	  	    memset((void *)virt_addr, 0, PAGE_SIZE);
		    map_phys_to_user_virt_addr((uint64_t)fault_addr, (uint64_t)new_page, (uint64_t *)curr_process->cr3);
		    break;
		}
	    }
	    else {
	       vma = vma->vm_next;
	    }
	}
    } 

    else if(phys_addr == 999 || vma == NULL) {
	/* Segmentation fault. Exit from the child now */
	kprintf("Segmentation fault\n");
	sys_exit();
    }

    set_CR3((uint64_t)curr_process->cr3);

    return;
}

void  init_mmap(pcb_t *process){
    uint64_t* phy_addr  =  (uint64_t *)allocate_virt_page();
    uint64_t  virt_addr = 0;
    vma_t    *vma_mmap;

// virt_addr = (uint64_t)(virtual_mmap | (uint64_t)phy_addr);
    virt_addr = (uint64_t)HEAP_ADDR;
    map_phys_to_user_virt_addr((uint64_t)virt_addr, (uint64_t)phy_addr, (uint64_t *)process->cr3); 
    vma_mmap  =(vma_t*) process->mm->vma;
    while(vma_mmap->type != HEAP){
    	vma_mmap = (vma_t*)vma_mmap->vm_next;
    }
    vma_mmap->vm_start = (uint64_t) virt_addr;
    vma_mmap->vm_end = (uint64_t) HEAP_END; 
    process->mm->mmap_start_addr = (uint64_t) virt_addr;
    process->mm->mmap_end_addr =  (uint64_t) HEAP_END;
    process->mm->mmap_alloc_addr = (uint64_t)virt_addr;
}

uint64_t mmap(int  size){
    uint64_t ending_addr = curr_process->mm->mmap_end_addr;
    uint64_t starting_addr = curr_process->mm->mmap_alloc_addr;
    if( (ending_addr - starting_addr) >= size){
        memset((void*)starting_addr, 0, (uint32_t)size); 
        curr_process->mm->mmap_alloc_addr += size;
    }
    return starting_addr;
}

void copy_parent_tables(uint64_t* cr3_addr) {
    uint64_t        pml4_index       = 0;
    uint64_t        pdpt_index       = 0;
    uint64_t        pd_index         = 0;
    uint64_t        pt_index         = 0;
    uint64_t       *pdpt;
    uint64_t       *c_pdpt;
    uint64_t        table_entry      = 0;
    uint64_t        phys_addr        = 0;
    uint64_t       *pd;
    uint64_t       *c_pd;
    uint64_t       *c_pt;

    cr3_addr = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)cr3_addr);
    __asm__ volatile("mov %%cr3, %0" : "=a" (pml4));
    pml4 = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)pml4);

    for (pml4_index = 0; pml4_index < 511; pml4_index++) {
        table_entry = pml4[pml4_index];
        if (table_entry & PT_PR) {
            table_entry = (uint64_t )remove_flag(table_entry);
            pdpt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
            c_pdpt = (uint64_t *)allocate_virt_page();
            phys_addr = (uint64_t)c_pdpt | (PT_PR | PT_WR | PT_U);
            cr3_addr[pml4_index] = phys_addr;
//	    pml4[pml4_index] = (uint64_t)table_entry | (PT_PR | PT_U | (0x1 << 9));
            c_pdpt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)c_pdpt);
            

            for (pdpt_index = 0; pdpt_index < 512; pdpt_index ++) {
                table_entry = pdpt[pdpt_index];
                if (table_entry & PT_PR) {
                    table_entry = (uint64_t )remove_flag(table_entry);
                    pd = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
                    c_pd = (uint64_t *)allocate_virt_page();
                    phys_addr =  (uint64_t)c_pd | (PT_PR | PT_WR | PT_U);
                    c_pdpt[pdpt_index] = phys_addr;
//		    pdpt[pdpt_index] = (uint64_t)table_entry | (PT_PR | PT_U | (0x1 << 9));
                    c_pd = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)c_pd);

                    for (pd_index = 0; pd_index < 512; pd_index++) {
                        table_entry = pd[pd_index];
                        if (table_entry & PT_PR) {
                            table_entry = (uint64_t )remove_flag(table_entry);
                            pt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
                            c_pt = (uint64_t *)allocate_virt_page();
                            phys_addr = (uint64_t)c_pt | (PT_PR | PT_WR | PT_U);
                            c_pd[pd_index] = phys_addr;
//			    pd[pd_index] = (uint64_t)table_entry | (PT_PR | PT_U | (0x1 << 9));
                            c_pt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)c_pt);

                            for (pt_index = 0; pt_index < 512; pt_index ++) {
                                table_entry = pt[pt_index];
                                if (table_entry & PT_PR) {
                                    table_entry = (uint64_t )remove_flag(table_entry);
				    pt[pt_index] = (uint64_t)table_entry | (PT_PR | PT_U | (0x1 << 9));
                                    table_entry = (uint64_t)table_entry | (PT_PR | PT_U | (0x1 << 9));
                                    c_pt[pt_index] = table_entry;
                                }
                            }
                        }
                    }

                }
            }
        }
    }
    cr3_addr[511] = pml4[511];
    return;
}

uint64_t virt_to_phys(uint64_t vAddress) {
    uint64_t     pAddr      = 0;
    uint64_t    *pml4 ;
    uint64_t    *pdpt;
    uint64_t    *pd;
    uint64_t    *pt;
    uint64_t     table_entry;
    uint64_t     pml4_index = (vAddress >> PML4_INDEX) & 0x1FF;
    uint64_t     pdpt_index = (vAddress >> PDPT_INDEX) & 0x1FF;
    uint64_t     pd_index   = (vAddress >> PD_INDEX) & 0x1FF;
    uint64_t     pt_index   = (vAddress >> PT_INDEX) & 0x1FF;

    __asm__ volatile("mov %%cr3, %0" : "=a" (pml4));
    pml4 = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)pml4);
    table_entry = pml4[pml4_index];

    if(table_entry & PT_PR) {
	table_entry = (uint64_t)remove_flag(table_entry);
	pdpt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
	table_entry = pdpt[pdpt_index];

   	if (table_entry & PT_PR) {
	    table_entry = (uint64_t)remove_flag(table_entry);
	    pd = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
	    table_entry = pd[pd_index];

	    if (table_entry & PT_PR) {
	        table_entry = (uint64_t)remove_flag(table_entry);
		pt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
		table_entry = pt[pt_index];

		if (table_entry & (PT_PR | (0x1 << 9))) {
		    table_entry = (uint64_t)remove_flag(table_entry);
		    pAddr = (uint64_t)table_entry;
		}
	        else if (table_entry & PT_PR) {
		    return 999;
		}
	    }
	}
    }
    return pAddr;
}

void free_page(uint64_t vAddress) {
    uint64_t    *pml4 ;
    uint64_t    *pdpt;
    uint64_t    *pd;
    uint64_t    *pt;
    uint64_t     table_entry;
    
    vAddress = (uint64_t)remove_flag(vAddress);
    uint64_t     pml4_index = (vAddress >> PML4_INDEX) & 0x1FF;
    uint64_t     pdpt_index = (vAddress >> PDPT_INDEX) & 0x1FF;
    uint64_t     pd_index   = (vAddress >> PD_INDEX) & 0x1FF;
    uint64_t     pt_index   = (vAddress >> PT_INDEX) & 0x1FF;

    __asm__ volatile("mov %%cr3, %0" : "=a" (pml4));
    pml4 = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)pml4);
    table_entry = pml4[pml4_index];

    if(table_entry & PT_PR) {
        table_entry = (uint64_t)remove_flag(table_entry);
        pdpt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
        table_entry = pdpt[pdpt_index];

        if (table_entry & PT_PR) {
            table_entry = (uint64_t)remove_flag(table_entry);
            pd = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
            table_entry = pd[pd_index];

            if (table_entry & PT_PR) {
                table_entry = (uint64_t)remove_flag(table_entry);
                pt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
                table_entry = pt[pt_index];

                if (table_entry & PT_PR ) {
                    table_entry = (uint64_t)remove_flag(table_entry);
		    set_bit(free_map, table_entry);
		    pt[pt_index] = 0;
		    table_entry = (uint64_t)(KERNEL_ADDR | table_entry);
		    memset((void *)table_entry, 0, PAGE_SIZE);
                }
            }
        }
    }
    return;
}

void free_kernel_page(uint64_t vAddress) {
    uint64_t    *pml4 ;
    uint64_t    *pdpt;
    uint64_t    *pd;
    uint64_t    *pt;
    uint64_t     table_entry;
    uint64_t     pml4_index = (vAddress >> PML4_INDEX) & 0x1FF;
    uint64_t     pdpt_index = (vAddress >> PDPT_INDEX) & 0x1FF;
    uint64_t     pd_index   = (vAddress >> PD_INDEX) & 0x1FF;
    uint64_t     pt_index   = (vAddress >> PT_INDEX) & 0x1FF;

    __asm__ volatile("mov %%cr3, %0" : "=a" (pml4));
    pml4 = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)pml4);
    table_entry = pml4[pml4_index];

    if(table_entry & PT_PR) {
        table_entry = (uint64_t)remove_flag(table_entry);
        pdpt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
        table_entry = pdpt[pdpt_index];

        if (table_entry & PT_PR) {
            table_entry = (uint64_t)remove_flag(table_entry);
            pd = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
            table_entry = pd[pd_index];

            if (table_entry & PT_PR) {
                table_entry = (uint64_t)remove_flag(table_entry);
                pt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);
                table_entry = pt[pt_index];

                if (table_entry & PT_PR ) {
                    table_entry = (uint64_t)remove_flag(table_entry);
                    set_bit(free_map, table_entry);
//                    pt[pt_index] = 0;
                    table_entry = (uint64_t)(KERNEL_ADDR | table_entry);
                    memset((void *)table_entry, 0, PAGE_SIZE);
                }
            }
        }
    }
    return;
}

void delete_pagetable(uint64_t pml4_addr) {
    uint64_t    table_entry;
    uint64_t    pml4_index;
    uint64_t    pdpt_index;
    uint64_t    pd_index;
    uint64_t    pt_index;
    uint64_t   *pml4;
    uint64_t   *pdpt;
    uint64_t   *pd;
    uint64_t   *pt;

    pml4 = (uint64_t*)((uint64_t)KERNEL_ADDR | pml4_addr);

    for (pml4_index = 0; pml4_index < 511; pml4_index++) {
        table_entry = pml4[pml4_index];
        if (table_entry & PT_PR) {
            table_entry = (uint64_t )remove_flag(table_entry);
            pdpt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);

            for (pdpt_index = 0; pdpt_index < 512; pdpt_index ++) {
                table_entry = pdpt[pdpt_index];
                if (table_entry & PT_PR) {
                    table_entry = (uint64_t )remove_flag(table_entry);
                    pd = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);

                    for (pd_index = 0; pd_index < 512; pd_index++) {
                        table_entry = pd[pd_index];
                        if (table_entry & PT_PR) {
                            table_entry = (uint64_t )remove_flag(table_entry);
                            pt = (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t)table_entry);

                            for (pt_index = 0; pt_index < 512; pt_index ++) {
                                table_entry = pt[pt_index];
                                if (table_entry & PT_PR) {
                                    table_entry = (uint64_t )remove_flag(table_entry);
                                    set_bit(free_map, table_entry);
                                    pt[pt_index] = 0;
                                    table_entry = (uint64_t)(KERNEL_ADDR | table_entry);
                                    memset((void *)table_entry, 0, PAGE_SIZE);
                                }
                            }
			    table_entry = pd[pd_index];
			    set_bit(free_map, table_entry);
			    pd[pd_index] = 0;
                            table_entry = (uint64_t)(KERNEL_ADDR | table_entry);
                            memset((void *)table_entry, 0, PAGE_SIZE);
                        }
                    }
                    table_entry = pdpt[pdpt_index];
                    set_bit(free_map, table_entry);
                    pdpt[pdpt_index] = 0;
                    table_entry = (uint64_t)(KERNEL_ADDR | table_entry);
                    memset((void *)table_entry, 0, PAGE_SIZE);
		}
	    }
            table_entry = pml4[pml4_index];
            set_bit(free_map, table_entry);
            pml4[pml4_index] = 0;
            table_entry = (uint64_t)(KERNEL_ADDR | table_entry);
            memset((void *)table_entry, 0, PAGE_SIZE);
	}
    }
    return;
}

