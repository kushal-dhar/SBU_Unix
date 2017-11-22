/*
 * The PIC_remap code has been taken unchanged from:
 * http://wiki.osdev.org/Expanded_Main_Page
 * 
 * and irq_function pointer routines has been taken unchanged from:
 * http://www.osdever.net/tutorials/
 */


#include <sys/pic.h>
#include <sys/defs.h>
#include <sys/picassem.h>
#include <sys/idt.h>
#include <sys/kprintf.h>
#include <sys/picassem.h>

/* call this from the assembly */
extern void isr32();
extern void isr33();
extern void isr14();

/* Array of function pointers to handle custom IRQ handlers  */
void *irq_routines[48]=
{
  0, 0, 0 , 0 , 0 , 0 , 0 , 0,
  0, 0, 0 , 0 , 0 , 0 , 0 , 0,
  0, 0, 0 , 0 , 0 , 0 , 0 , 0,
  0, 0, 0 , 0 , 0 , 0 , 0 , 0,
  0, 0, 0 , 0 , 0 , 0 , 0 , 0,
  0, 0, 0 , 0 , 0 , 0 , 0 , 0
 
};

/* To handle particular handler with a functional routines */
void irq_set(int req, void (*handler)(struct regis *r)){
	irq_routines[req] = handler;
}

/*clean function handlers */

void irq_reset(int req){
	irq_routines[req] = 0;
}

/*Mapping of PIC to 32 to 47 */
void PIC_remap(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

void PIC_sendEOI(uint8_t irq)
{
     if(irq >= 8)
             outb(PIC2_COMMAND,PIC_EOI);
     outb(PIC1_COMMAND,PIC_EOI);
}

void init_picirr(){
    PIC_remap(0x20, 0x28);
    /** Adding ISR **/
    /*This is for system timer */
    set_idt_gate(INTR_32 ,(uint64_t)isr32, IDT_SEL , INTERRUPT_GATE);
    /*This is for keyboard interrupt */
    set_idt_gate(INTR_33 ,(uint64_t)isr33, IDT_SEL , INTERRUPT_GATE);
    /* This if for Page Fault Handler */
    set_idt_gate(INTR_14 ,(uint64_t)isr14, IDT_SEL , INTERRUPT_GATE);
}

void intr_handler(regis* regs){
   /*  It is for handling the custom interrupts */
    void (*handler) (regis *regs)= NULL;
     uint64_t sys = 0;
     __asm__ volatile(
       "movq %%rax , %0\n\t"
        : "=a"(sys)
        :
        :
	);
//     if (sys < 32) {
	handler = irq_routines[sys];
#if 0
     }
     else {
         handler = irq_routines[sys - 32];
     }
#endif
     if(handler)
    { 
       handler(regs);
    }
   /* sending end of interrupt */
#if 0
    if (sys < 32) {
#endif
        PIC_sendEOI(sys);
#if 0
    }
    else {
        PIC_sendEOI(sys-32);  
    }
#endif
}

