#ifndef _IDT_H
#define _IDT_H

#include <sys/defs.h>

/*Making an idt table */
typedef struct idt_entry{
  uint16_t addr_low;
  uint16_t selector;
  uint8_t always0;
  uint8_t flags;
  uint16_t addr_mid;
  uint32_t addr_high;
  uint32_t reserved;
}__attribute__((packed)) idt_entry;

/*  pointer of the IDT table */
typedef struct idt_ptr
{
  uint16_t limit;
  uint64_t base_addr;
}__attribute__((packed)) idt_ptr;

/* Show C our stack frame */
typedef struct regis {
//  uint64_t rax,rbx,rcx,rdx,rsi,rdi,r8,r9,r10,r11;
 uint32_t gs,fs,es,ds;
   uint8_t edi,esi,ebp,esp,ebx,edx,ecx,eax;
   uint8_t int_no,err_code;
   uint8_t eip,cs,eflags,useresp,ss;
} regis ;

#define IDT_SEL 0x08
#define INTERRUPT_GATE 0x8E
#define INTERRUPT_LIMIT 256
#define INTR_0 32
#define INTR_1 33
#define INTR_14 -18
void set_idt_gate(uint8_t interrupt_num,uint64_t base_addr, uint16_t sel, uint8_t gate );

void init_idt();
#endif
