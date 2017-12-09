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
   uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
   uint64_t rbp;
   uint64_t rdi, rsi;
   uint64_t rdx, rcx, rbx, rax;
   uint64_t rip, cs, eflags, rsp, ss;
// uint32_t gs,fs,es,ds;
//   uint8_t edi,esi,ebp,esp,ebx,edx,ecx,eax;
//   uint8_t eip,cs,eflags,useresp,ss;
} regis ;


#define IDT_SEL 0x08
#define INTERRUPT_GATE 0x8E
#define INTERRUPT_LIMIT 256
#define INTR_32 32
#define INTR_33 33
#define INTR_14 14
#define INTR_128 128
void set_idt_gate(uint8_t interrupt_num,uint64_t base_addr, uint16_t sel, uint8_t gate );

void init_idt();
#endif
