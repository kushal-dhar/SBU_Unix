#include <sys/idt.h>
#include <sys/kprintf.h>
#include <sys/defs.h>

struct idt_entry entries[256];
struct idt_ptr idtp;

/*call this from the assembly language  */
extern void idt_load();

void set_idt_gate(uint8_t num,uint64_t base_addr, uint16_t sel, uint8_t gate ){
    entries[num].addr_low = base_addr & 0xFFFF;
    entries[num].addr_mid = (base_addr >> 16 ) & 0xFFFF;
    entries[num].addr_high = (base_addr >>32) & 0xFFFFFFFF;
    entries[num].selector = sel;
    entries[num].always0 = 0;
    entries[num].flags = gate;
}

void init_idt(){
    idtp.limit = sizeof(struct idt_entry) *  INTERRUPT_LIMIT  - 1;
    idtp.base_addr = (uint64_t) &entries;

    /** pushing idt to the lidt register **/
    idt_load();
}

