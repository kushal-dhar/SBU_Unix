#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/timer.h>
#include <sys/kb.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <sys/process.h>
#include <sys/elf64.h>

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
char buffer[100];
void caller(){
for(int i =0; i <30;i++){
kprintf("%d\n",i);
}
}

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  uint64_t max_size = 0;
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap, smap_max = {0};
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->length);
      if (smap->length > max_size) {
	max_size = smap->length;
	smap_max.base = smap->base;
	smap_max.length = smap->length;
      }
      //init_phys_mem((uint64_t)physbase, (uint64_t)physfree, smap->base);
    }
  }
  init_paging((uint64_t)physbase, (uint64_t)physfree, smap_max.base, smap_max.length);
  kprintf("physfree %p\n", (uint64_t)physfree);
  kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
  
  setup_page_tables((uint64_t)physbase, (uint64_t)physfree);
  set_identity_paging();
  load_CR3();
  kprintf("Printing after CR3 called");
  init_idt();

  init_picirr();

  init_timer(1000);
  kb_init();
  kprintf("Printing something\n");
  __asm__ volatile("sti"); 
  uint64_t *addr = (uint64_t *)kmalloc(6000);
  kprintf("addrs: %d\n",addr);
//  is_file_exist("hello");
  init_tarfs();
  enable_page_fault();
//  kprintf("printing value at %p\n",*(uint64_t *)0x888FF292001);
  int fd = open("bin/hello", 2);
  read(fd, buffer, 100);
  kprintf("file: %s\n",buffer);
  //mm_struct_t *mm = (mm_struct_t *)kmalloc(4096);
//  load_binaries(fd, mm);
//  caller();  
//  pcb_t *user_process = create_user_process("bin/hello");
//  switch_to_ring3((pcb_t *)user_process);

  create_kernel_thread();
//caller();
  while(1);

}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__(
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt();
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
/*     init_idt();

     init_picirr();

     init_timer(1000);
     kb_init();
     __asm__("sti"); */
  
  for(
    temp1 = "", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;

  while(1);
}
