#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/timer.h>
#include <sys/paging.h>
#include <sys/paging_helper.h>
#include <sys/process.h>
#include <sys/utils.h>
#include <sys/string.h>

extern tarfile_t global_tarfs[100];

/*
 * Create a new VMA, allocate start and end address and return
 */
vma_t* create_vma(uint64_t vm_start, uint64_t vm_end, int p_flags) {
    vma_t *vma;
    
    vma = (vma_t *)kmalloc(sizeof(vma_t *));
    vma->vm_start = vm_start;
    vma->vm_end = vm_end;
    vma->flags = p_flags | PT_U;
    return vma;
}

int add_mmstruct(mm_struct_t *mm, vma_t *vma) {
    vma_t    *curr;
 
    if (vma->vm_start >= vma->vm_end) {
	return -1;
    }

    curr = mm->vma;
    /* Insert at the 1st position and return */
    if (!mm->vma || vma->vm_end <= mm->vma->vm_end) {
        vma->vm_next = mm->vma;
        mm->vma = vma;
	vma->vm_mm = mm;
        mm->vma_count ++;
 	return 0;
    }

    for (; curr != NULL; curr = curr->vm_next) {
	if (curr->vm_next != NULL && curr->vm_next->vm_start >= vma->vm_end) {
	    vma->vm_next = curr->vm_next;
	    curr->vm_next = vma;
	    vma->vm_mm = mm;
	    mm->vma_count ++;
	    return 0;
	}
	else if (curr->vm_next == NULL) {
	    curr->vm_next = vma;
	    vma->vm_mm = mm;
	    mm->vma_count ++;
	    return 0;
	}
    }
    return -1;
}

int mmap_alloc(mm_struct_t *mm, int fp, uint64_t file_start, uint64_t size, uint64_t vm_start, uint64_t vm_end, int p_flags) {
    vma_t *vma_alloc;

    vma_alloc = create_vma(vm_start, vm_end, p_flags);
//    vma_alloc->file_pt = fp;
    vma_alloc->file_start = file_start;
    vma_alloc->file_size = size;
    vma_alloc->vm_next = NULL;
     
    add_mmstruct(mm, vma_alloc);
    return 1;
}

