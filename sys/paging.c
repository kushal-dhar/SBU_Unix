#include <sys/defs.h>
#include <string.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <sys/kprintf.h>

uint64_t *pml4;
uint64_t *pdpt;
uint64_t *pd;
uint64_t *pt;

extern char kernmem;
uint64_t *free_map;
uint64_t cr3;
uint64_t physfree_start, physfree_end;
uint64_t virt_addr = (uint64_t)KERNEL_BASE;


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
    pdpt_entry |= (PT_PR | PT_WR | PT_U);

    pml4[pml4_index] = pdpt_entry;
    return ;
}

void pd_allocate(uint64_t* pdpt, uint64_t pdpt_index) {
    pd = (uint64_t *)allocate_page();
    uint64_t pd_entry = (uint64_t)pd;
    pd_entry |= (PT_PR | PT_WR | PT_U);

    pdpt[pdpt_index] = pd_entry;
    return;
}

void pt_allocate(uint64_t* pd, uint64_t pd_index) {
    pt = (uint64_t *)allocate_page();
    uint64_t pt_entry = (uint64_t)pt;
    pt_entry |= (PT_PR | PT_WR | PT_U);

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
	address_ptr |= (PT_PR | PT_WR | PT_U);
	pt[pt_index] = address_ptr;
    }

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
    address_ptr |= (PT_PR | PT_WR | PT_U);
    pt[pt_index] = address_ptr; 

    vAddress = KERNEL_ADDR | (uint64_t)pml4;
    map_virt_to_phys_addr((uint64_t)vAddress, (uint64_t)pml4);
    vAddress = KERNEL_ADDR | (uint64_t)pdpt;
    map_virt_to_phys_addr((uint64_t)vAddress, (uint64_t)pdpt);
    vAddress = KERNEL_ADDR | (uint64_t)pd;
    map_virt_to_phys_addr((uint64_t)vAddress, (uint64_t)pd);
    vAddress = KERNEL_ADDR | (uint64_t)pt;
    map_virt_to_phys_addr((uint64_t)vAddress, (uint64_t)pt);
    
    //set_identity_paging();
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
	pml4[pml4_index] = (uint64_t)pdpt | (PT_PR | PT_WR | PT_U);
    }
    
    pdpt_addr = pdpt[pdpt_index];
    if (pdpt_addr & PT_PR) {
	pd = (uint64_t *)remove_flag(pdpt_addr);
    }
    else {
    /* If page descriptor entry is not allocated yet, allocate a new entry */
	pd = (uint64_t *)allocate_page();
	pdpt[pdpt_index] = (uint64_t)pd | (PT_PR | PT_WR | PT_U);
    }
    pd_addr = pd[pd_index];
    if (pd_addr & PT_PR) {
        pt = (uint64_t *)remove_flag(pd_addr);
    } 
    else {
    /* If page table entry is not allocated yet, allocate a new entry */
	pt = (uint64_t *)allocate_page();
	pd[pd_index]=(uint64_t)pt | (PT_PR | PT_WR | PT_U);
    }
    
    pt_addr = phys;
    pt_addr |= (PT_PR | PT_WR | PT_U);
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

    uint64_t pml4_addr = pml4[pml4_index];
    uint64_t pdpt_addr;
    uint64_t pd_addr;
    uint64_t pt_addr;

    if (pml4_addr & PT_PR) {
        pml4_addr = (uint64_t )remove_flag(pml4_addr);
    	pdpt = (uint64_t *)pml4_addr;
        pdpt = (uint64_t *)((uint64_t)KERNEL_ADDR |(uint64_t)pdpt);
    }
    else {
    /* If pdpt entry is not allocated yet, allocate a new entry */
        pdpt = (uint64_t *)allocate_virt_page();
        pml4[pml4_index] = (uint64_t)pdpt | (PT_PR | PT_WR | PT_U);
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
        pd[pd_index]=(uint64_t)pt | (PT_PR | PT_WR | PT_U);
	pt =  (uint64_t *)((uint64_t)KERNEL_ADDR | (uint64_t )pt);
    }

    pt_addr = phys;
    pt_addr |= (PT_PR | PT_WR | PT_U);
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
    uint64_t   ret_addr;

    if (size % PAGE_SIZE == 0) {
	num_page = size / PAGE_SIZE;
    }
    else {
 	num_page = (size / PAGE_SIZE) + 1;
    }
    ret_addr = virt_addr + 0x1000;
    
    for (i = 0; i < num_page; i++) {
	virt_addr += 0x1000;
	page_addr = (uint64_t )allocate_virt_page();
        map_phys_to_virt_addr((uint64_t)virt_addr, (uint64_t)page_addr);
	memset((void*)virt_addr, 0, (uint32_t)PAGE_SIZE);
    }
   
    return (uint64_t *)ret_addr; 
}
