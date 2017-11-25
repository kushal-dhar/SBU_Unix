#include <sys/defs.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <string.h>

extern uint64_t *pml4;
extern uint64_t *pdpt;
extern uint64_t *pd;
extern uint64_t *pt;

extern char kernmem;
extern uint64_t *free_map;
extern uint64_t cr3;
extern uint64_t physfree_start;
extern uint64_t  physfree_end;
extern uint64_t virt_addr;


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
    uint64_t pml4_addr= cr3;
    __asm__ volatile("movq %0, %%cr3":: "b"(pml4_addr));
    pml4 = (uint64_t *)(KERNEL_ADDR | (uint64_t)pml4);
    free_map = (uint64_t *) (0xFFFFFFFF80000000UL | (uint64_t)free_map);
}

void set_CR3(uint64_t pml4_addr) {
    __asm__ volatile("movq %0, %%cr3":: "b"(pml4_addr));
    //pml4 = (uint64_t *)(KERNEL_ADDR | (uint64_t)pml4);
    //free_map = (uint64_t *) (0xFFFFFFFF80000000UL | (uint64_t)free_map);
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

void memcpy(volatile void *src, volatile void *dest, uint32_t n) {
    char *temp1 = (char *)src;
    char *temp2 = (char *)dest;
    int i = 0;

    for (i = 0; i < n; i++) {
     	temp2[i] = temp1[i];
    }
}

int get_index(uint64_t address, int offset) {
    address = (address >> offset) & 0x1FF;
    return address;
}

volatile uint64_t get_CR3_address() {
    volatile uint64_t cr3_addr;
    __asm__ volatile("mov %%cr3, %0" : "=r" (cr3_addr));
    return cr3_addr;
}

/* Mask the LSB 12 bit and return the address */
uint64_t remove_flag(uint64_t addr) {
    return (uint64_t)(addr & 0xFFFFFFFFFFFFF000UL);
}

