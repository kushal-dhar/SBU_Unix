#ifndef _ELF64_H
#define _ELF64_H

#include <sys/defs.h>
#include <sys/process.h>

#define EI_NIDENT 16

#define ELFMAG0 0x7F
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define PF_R     0x4
#define PF_W     0x2
#define PF_X     0x1

#define PT_NULL     0
#define PT_LOAD     1
#define PT_DYNAMIC  2

#define TEXT    0
#define DATA    1
#define HEAP    2
#define STACK   3
#define UNDEF   4

typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Lword;
typedef uint64_t Elf64_Off;
typedef uint32_t Elf64_Sword;
typedef uint64_t Elf64_Sxword;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;


enum ELF_Ident{
EI_MAG0        = 0,
EI_MAG1        = 1,
EI_MAG2        = 2,
EI_MAG3        = 3,
EI_CLASS       = 4,
EI_DATA        = 5,
EI_VERSION     = 6,
EI_OSABI       = 7,
EI_ABIVERSION  = 8,
EI_PAD         = 9
};

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  Elf64_Half    e_type;
  Elf64_Half    e_machine;
  Elf64_Word    e_version;
  Elf64_Addr    e_entry;
  Elf64_Off     e_phoff;
  Elf64_Off     e_shoff;
  Elf64_Word    e_flags;
  Elf64_Half    e_ehsize;
  Elf64_Half    e_phentsize;
  Elf64_Half    e_phnum;
  Elf64_Half    e_shentsize;
  Elf64_Half    e_shnum;
  Elf64_Half    e_shstrndx;
} Elf64_Ehdr;

typedef struct {
  Elf64_Word    p_type;
  Elf64_Word    p_flags;
  Elf64_Off     p_offset;
  Elf64_Addr    p_vaddr;
  Elf64_Addr    p_paddr;
  Elf64_Xword   p_filesz;
  Elf64_Xword   p_memsz;
  Elf64_Xword   p_align;
} Elf64_Phdr;


extern int elf_check_header(Elf64_Ehdr *elfhdr);
extern int load_binaries(pcb_t *proc, uint64_t *elf_start);
#endif
