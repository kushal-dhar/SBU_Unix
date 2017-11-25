#include <sys/elf64.h>
#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/idt.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <sys/process.h>
#include <sys/vma.h>

extern tarfile_t global_tarfs[100];
extern pcb_t *first_process;
extern pcb_t *curr_process;

int elf_check_header(Elf64_Ehdr *elfhdr) {
    if (! elfhdr) {
	return 0;
    }
    else if (elfhdr->e_ident[EI_MAG0] != ELFMAG0) {
	kprintf("ELF Header EI_MAG0 incorrect \n");
	return 0;
    }
    else if (elfhdr->e_ident[EI_MAG1] != ELFMAG1) {
        kprintf("ELF Header EI_MAG1 incorrect \n");
        return 0;
    }
    else if (elfhdr->e_ident[EI_MAG2] != ELFMAG2) {
        kprintf("ELF Header EI_MAG2 incorrect \n");
        return 0;
    }
    else if (elfhdr->e_ident[EI_MAG3] != ELFMAG3) {
        kprintf("ELF Header EI_MAG3 incorrect \n");
        return 0;
    }
 
    return 1;
}

int load_binaries(pcb_t *proc, uint64_t *elf_start) {
    Elf64_Ehdr *elf_hdr;
    Elf64_Phdr *elf_phdr;
//    char       *temp1;
//    char       *temp2;
    int         i          = 0;
//    int         iterator   = 0;
    int         p_flags    = 0;
    int         ret_val    = 0;
    uint64_t    phdr_start = 0;
    uint64_t    phdr_end   = 0;
//    uint64_t    size       = 0;

//    elf_hdr = (Elf64_Ehdr *)global_tarfs[fp].addr;
    elf_hdr = (Elf64_Ehdr *)elf_start;

    if (!elf_check_header(elf_hdr)) {
        return -1;
    }

    kprintf("elf_hdr: %d\n",elf_hdr->e_entry);
    proc->rip = (uint64_t)elf_hdr->e_entry;
    for (i = 0; i < elf_hdr->e_phnum; i++) {
	elf_phdr = (Elf64_Phdr *)((uint64_t)elf_hdr + elf_hdr->e_phoff) + i;
        phdr_start = (elf_phdr->p_vaddr / PAGE_SIZE) * PAGE_SIZE;
        phdr_end = elf_phdr->p_vaddr + elf_phdr->p_memsz;

	if (elf_phdr->p_type == PT_LOAD) {
            kprintf("p_vaddr: %d\n",elf_phdr->p_vaddr);
	    p_flags = (elf_phdr->p_flags & PF_W) ? PT_WR:     0;
//	    phdr_start = (elf_phdr->p_vaddr / PAGE_SIZE)     * PAGE_SIZE;
//	    phdr_end = elf_phdr->p_vaddr + elf_phdr->p_memsz;
	    kprintf("p_flags: %d\n",p_flags);
	    kprintf("start: %d\n",phdr_start);
	    kprintf("end: %d\n",phdr_end);
//            ret_val = mmap_alloc(mm, fp, elf_phdr->p_offset, elf_phdr->p_filesz, elf_phdr->p_vaddr, phdr_end, p_flags);
/*	    if (ret_val == -1) {
		kprintf("Memory Allocation failed\n");
		return ret_val;
	    } */
	    vma_t *vma = (vma_t *)umalloc(proc, 100 );
            vma->vm_start = elf_phdr->p_vaddr;
            vma->vm_end = phdr_end;

	    ret_val = add_mmstruct(proc->mm, vma);

	    if (ret_val == -1) {
		kprintf("Unable to add to mmstruct\n");
		return -1;
	    }
	    
	    //size = (phdr->end - phdr->start);
	    //size = ((size % PAGE_SIZE) == 0) ? size : ((size / PAGE_SIZE) + 1) * PAGE_SIZE;
	    while (phdr_start <= phdr_end) {
	uint64_t *page = (uint64_t *)allocate_virt_page();
		map_phys_to_user_virt_addr((uint64_t)phdr_start, (uint64_t)page, (uint64_t *)proc->cr3);
// 	        map_phys_to_virt_addr((uint64_t)phdr_start, (uint64_t)page);
		phdr_start += PAGE_SIZE;
	    }
	    vma->flags = elf_phdr->p_flags;
	    vma->vm_next = NULL;
	
   	    if ((elf_phdr->p_flags == (PF_R | PF_X)) || (elf_phdr->p_flags == (PF_R | PF_W))) {
	    	proc->mm->code_start = phdr_start;
		proc->mm->code_end = phdr_end;
		vma->file = (tarfile_t *)umalloc(proc, 100);
	        vma->file_start = elf_phdr->p_offset;
	        vma->file_size = elf_phdr->p_filesz;
//		set_CR3((uint64_t)proc->cr3);
	        memcpy((void *)(uint64_t)elf_hdr + elf_phdr->p_offset, (void *)vma->vm_start,elf_phdr->p_filesz);
/*		temp1 = (char *)elf_hdr + elf_phdr->p_offset;
		temp2 = (char *)vma->vm_start;
 	 	for (iterator = 0; iterator < elf_phdr->p_filesz; iterator++) {
		    temp2[iterator] = temp1[iterator];
		} */
//		set_CR3((uint64_t)first_process->cr3);
	        if (elf_phdr->p_flags == (PF_R | PF_X)) {
		    vma->type = TEXT;
	        }
	        else {
		    vma->type = DATA;
	        }
	    }
	}		
    }
    
    /* Heap allocation */
    uint64_t *page = (uint64_t *)allocate_virt_page();
    vma_t *heap_vma = (vma_t *)((uint64_t)USER_VIRT_ADDR | (uint64_t)page);
    map_phys_to_user_virt_addr((uint64_t)heap_vma, (uint64_t)page, (uint64_t *)proc->cr3);
//    vma_t *heap_vma = (vma_t *)umalloc(proc, 4096);
    heap_vma->vm_start = (uint64_t)page;
    heap_vma->vm_end = (uint64_t)page + PAGE_SIZE;
    ret_val = add_mmstruct(proc->mm, heap_vma);

    if (ret_val == -1) {
        kprintf("Unable to add to mmstruct\n");
        return -1;
    }
    heap_vma->flags = (PF_R | PF_W);
    heap_vma->type = HEAP;
    heap_vma->file = NULL;
    heap_vma->vm_next = NULL;

    /* Stack allocation */
/*    vma_t *stack_vma = (vma_t *)umalloc(proc, 4096);
    stack_vma->start = */

    return 0;
}
