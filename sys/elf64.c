#include <sys/elf64.h>
#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/idt.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <sys/process.h>

extern tarfile_t global_tarfs[100];

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

int load_binaries(int fp, mm_struct_t *mm) {
    Elf64_Ehdr *elf_hdr;
    Elf64_Phdr *elf_phdr;
    int         i          = 0;
#if 0
    int         p_flags    = 0;
    int         ret_val    = 0;
    uint64_t    phdr_end   = 0;
#endif

    elf_hdr = (Elf64_Ehdr *)global_tarfs[fp].addr;

    if (!elf_check_header(elf_hdr)) {
        return -1;
    }

    kprintf("elf_hdr: %d\n",elf_hdr->e_entry);
    mm->rip = (uint64_t)elf_hdr->e_entry;
   
    for (i = 0; i < elf_hdr->e_phnum; i++) {
	elf_phdr = (Elf64_Phdr *)((uint64_t)elf_hdr + elf_hdr->e_phoff) + i;
	if (elf_phdr->p_type == PT_LOAD) {
#if 0
	    p_flags = (elf_phdr->p_flags & PF_W) ? RW_FLAG: 0;
	    pdhr_end = elf_phdr->p_vaddr + p_memsz;
            ret_val = mmap_alloc(mm, fp, elf_phdr->p_offset, elf_phdr->p_filesz, elf_phdr->p_vaddr, phdr_end, p_flags);
	    if (ret_val == -1) {
		kprintf("Memory Allocation failed\n");
		return ret_val;
	    }
#endif
	}	
    }
    return 0;
}
