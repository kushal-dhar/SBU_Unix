#ifndef __VMA_H
#define __VMA_H

#include <sys/process.h>

extern vma_t* create_vma(uint64_t vm_start, uint64_t vm_end, int p_flags);
extern int add_mmstruct(mm_struct_t *mm, vma_t *vma);
extern int mmap_alloc(mm_struct_t *mm, int fp, uint64_t file_start, uint64_t size, uint64_t vm_start, uint64_t vm_end, int p_flags);


#endif
