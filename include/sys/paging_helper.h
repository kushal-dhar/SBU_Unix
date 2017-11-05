#include <sys/paging.h>
#include <sys/defs.h>

extern void set_bit(uint64_t *map, uint64_t val);
extern void unset_bit(uint64_t *map, uint64_t val);
extern void load_CR3();
extern void* memset(void *ptr, int x, uint32_t n);
extern int get_index(uint64_t address, int offset);
extern uint64_t get_CR3_address();
extern uint64_t remove_flag(uint64_t addr);
