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

/*
 * Set the specific bit in val-th position
 */
void set_bit(uint64_t *map, uint64_t val) {
    map[val/64] |= (1ull << (val%64));
}

/*
 * Un-Set the specific bit in val-th position
 */
void unset_bit(uint64_t *map, uint64_t val) {
    map[val/64] &= ~(1ull << (val%64));
}

void load_CR3() {
    uint64_t plm4_addr= cr3;
    __asm__ volatile("movq %0, %%cr3":: "b"(plm4_addr));	
}

void* memset(void *ptr, int x, uint32_t n) {
    char *temp = (char*)ptr;
    while(n > 0) {
        *temp = x;
        temp++;
        n --;
    }
    return ptr;
}

/* Mask the LSB 12 bit and return the address */
uint64_t remove_flag(uint64_t addr) {
    return (addr & ~(0xFFF));
}

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
    kprintf("base %x length %x\n",base,length);
    physfree_start += (uint64_t)(256 * 64/8);
    //physfree_start += (uint64_t)8192;
    kprintf("physfree_start: %x\n",physfree_start);
    physfree_end = end;

    while (start < end) {
	address = start * PAGE_SIZE;
 	if (address < physfree_start) {
	/* For address lower than 1MB, dont set free page-table */
	    start ++;
	    //kprintf("address now: %x\n",address);
	    continue;
	}
	/*if (address > physbase && address < physfree) {
	    start ++;
	    continue;
	}*/
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

void setup_page_tables(uint64_t phys_base, uint64_t phys_free) {
    uint64_t  vAddress = (uint64_t)&kernmem;
    uint64_t  pml4_index = get_index(vAddress, PML4_INDEX);
    uint64_t  pdpt_index = get_index(vAddress, PDPT_INDEX);
    uint64_t  pd_index = get_index(vAddress, PD_INDEX);
    uint64_t  pt_index = get_index(vAddress, PT_INDEX);
    uint64_t  address_ptr = phys_base;

    pml4 = (uint64_t *)allocate_page();
    address_ptr = (uint64_t)pml4;
    //address_ptr |= (PT_PR | PT_WR | PT_U);
    //pml4 = (uint64_t *)0x270000;
    cr3 = (uint64_t)address_ptr;
    kprintf("cr3 address now: %x\n",cr3);

    pdpt_allocate(pml4, pml4_index);
    pd_allocate(pdpt, pdpt_index);
    pt_allocate(pd, pd_index);

    /* Allocate page_table entries for kernel memory */
    kprintf("physfree start here: %x\n",physfree_start);
    for(; phys_base < physfree_start; phys_base += 0x1000, vAddress += 0x1000) {
	pt_index = get_index(vAddress, PT_INDEX);
	address_ptr = phys_base;
	address_ptr |= (PT_PR | PT_WR | PT_U);
	pt[pt_index] = address_ptr;
    }

    vAddress = (uint64_t)&kernmem;
    vAddress &= 0xFFFFF000000;
    vAddress |= 0xb8000;
    phys_base = 0xb8000;
    //pml4_index = get_index(vAddress, PML4_INDEX);
    //pdpt_index = get_index(vAddress, PDPT_INDEX);
    //pd_index = get_index(vAddress, PD_INDEX);
    kprintf("pd_index: %d\n",pd_index);
    pd_index = get_index(vAddress, PD_INDEX);
    kprintf("pd_index now: %d\n",pd_index);
    pt_index = get_index(vAddress, PT_INDEX);
    pt_allocate(pd, pd_index);

    //pt_allocate(pd, pd_index);
    kprintf("I am here");
    address_ptr = phys_base;
    address_ptr |= (PT_PR | PT_WR | PT_U);
    pt[pt_index] = address_ptr; 

    return;
}

void map_virt_to_phys_addr(uint64_t virtual, uint64_t phys) {
    uint64_t pml4_index = get_index(virtual, PML4_INDEX);
    uint64_t pdpt_index = get_index(virtual, PDPT_INDEX);
    uint64_t pd_index = get_index(virtual, PD_INDEX);
    uint64_t pt_index = get_index(virtual, PT_INDEX);
    
    uint64_t pml4_addr = pml4[pml4_index];
    uint64_t pdpt_addr;
    uint64_t pd_addr;
    uint64_t pt_addr;

    if (pml4_addr & PT_PR) {
	pdpt = (uint64_t *)remove_flag(pml4_addr);
    }
    pdpt_addr = pdpt[pdpt_index];
    if (pdpt_addr & PT_PR) {
	pd = (uint64_t *)remove_flag(pdpt_addr);
    }
    pd_addr = pd[pd_index];
    if (pd_addr & PT_PR) {
        pt = (uint64_t *)remove_flag(pd_addr);
    }
    pt_addr = phys;
    pt_addr |= (PT_PR | PT_WR | PT_U);
    pt[pt_index] = pt_addr;

    return;
}

void set_identity_paging(uint64_t phys_base) {
    uint64_t vAddr = (uint64_t)&kernmem;
    uint64_t pAddr = 0;
    
    vAddr -= phys_base;

    for (; pAddr < physfree_end; pAddr += 0x1000, vAddr += 0x1000) {
	map_virt_to_phys_addr(vAddr, pAddr);
    }
    map_virt_to_phys_addr(0xFFFFFFFF800B8000, 0xB8000);
    free_map = (uint64_t*)(0xFFFFFFFF80000000UL | (uint64_t) free_map); 

    return;
}


