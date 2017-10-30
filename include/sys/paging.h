#ifndef __PAGING_H_
#define __PAGING_H_

#if 0
#define PML4_INDEX(x)	(((x) >> PML4SHIFT) & 0x1FF)
#endif

#define PAGE_SIZE 4096

#define PML4_INDEX    39
#define PDPT_INDEX    30
#define PD_INDEX      21
#define PT_INDEX      12

#define PT_PR  0x01
#define PT_WR  0x02
#define PT_U   0x04

extern void set_bit(uint64_t *map, uint64_t val);
extern void unset_bit(uint64_t *map, uint64_t val);
extern void load_CR3();
extern void init_paging(uint64_t physbase, uint64_t physfree, uint64_t base, uint64_t length);
extern int get_firstpage();
extern uint64_t* allocate_page();
extern void pdpt_allocate(uint64_t *pml4, uint64_t pml4_index);
extern void pd_allocate(uint64_t* pdpt, uint64_t pdpt_index);
extern void pt_allocate(uint64_t* pd, uint64_t pd_index);
extern void setup_page_tables(uint64_t physbase, uint64_t physfree);
extern uint64_t remove_flag(uint64_t addr) ;
extern void map_virt_to_phys_addr(uint64_t virtual, uint64_t phys);
extern void set_identity_paging(uint64_t phys_base);

#endif
