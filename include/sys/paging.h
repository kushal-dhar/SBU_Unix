#ifndef __PAGING_H_
#define __PAGING_H_

#define KERNEL_ADDR 0xFFFFFFFF80000000UL
#define KERNEL_BASE 0xFFFFFFFF80600000UL
#define PHYS_ADDR 0x0UL
#define VIDEO_ADDR 0xFFFFFFFF800B8000
#define VIDEO_PHYS_ADDR 0xB8000

#define PAGE_SIZE 4096

#define PML4_INDEX    39
#define PDPT_INDEX    30
#define PD_INDEX      21
#define PT_INDEX      12

#define PT_PR  0x01
#define PT_WR  0x02
#define PT_U   0x04

extern void init_paging(uint64_t physbase, uint64_t physfree, uint64_t base, uint64_t length);
extern int get_firstpage();
extern uint64_t* allocate_page();
extern uint64_t* allocate_virt_page();
extern void pdpt_allocate(uint64_t *pml4, uint64_t pml4_index);
extern void pd_allocate(uint64_t* pdpt, uint64_t pdpt_index);
extern void pt_allocate(uint64_t* pd, uint64_t pd_index);
extern void setup_page_tables(uint64_t physbase, uint64_t physfree);
extern void map_virt_to_phys_addr(uint64_t virtual, uint64_t phys);
extern void map_virt_to_phys_addr_afterpaging(uint64_t vAddress, uint64_t phys);
extern void set_identity_paging();
extern uint64_t* kmalloc(int size);

#endif
