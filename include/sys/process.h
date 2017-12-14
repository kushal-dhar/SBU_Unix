#ifndef _PROCESS_H
#define _PROCESS_H

#include <sys/tarfs.h>

#define MAX_PROCESS_COUNT      64
#define MAX_FD_COUNT           100

// Task States
#define  TASK_NEW         1
#define  TASK_READY       2
#define  TASK_RUNNING     3
#define  TASK_WAITING     4
#define  TASK_STOPPED     5
#define  TASK_ZOMBIE      6

#define ROOT "rootfs/"

typedef struct process_ctrl_blk pcb_t;
typedef struct mm_struct mm_struct_t;
typedef struct vma vma_t;
typedef struct registers register_t;

struct mm_struct {
    uint64_t  code_start, code_end, data_start, data_end;
    uint64_t  start_brk, stack_start;
    uint64_t  arg_start, arg_end;
    uint64_t  env_start, env_end;
    uint64_t  rip;
    uint64_t  vm_total;
    vma_t    *vma;
    uint64_t  vma_count;
    uint64_t  mmap_start_addr;
    uint64_t  mmap_end_addr;
    uint64_t  mmap_alloc_addr;
}__attribute__((packed));

struct vma {
    mm_struct_t   *vm_mm;
    vma_t         *vm_next;
    uint64_t       vm_start;
    uint64_t       vm_end;
    uint64_t       type;
    uint64_t       flags;
    tarfile_t     *file;
    uint64_t       file_start;
    uint64_t       file_size;
};

struct registers {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp, r8, r9, r10, r11;
}__attribute__((packed));

//PCB for each processes
struct process_ctrl_blk {
    char          kernel_stack[4096];
    int           pid;
    int           ppid;
    char          p_name[30];
    uint64_t      init_kernel;
    uint64_t      user_stack;
    uint64_t      rsp;
    uint64_t      kern_rsp;
    tarfile_t     fd[20];
    uint64_t      file_count;
    uint64_t      rip;
    uint64_t      cr3;
    int           state;
    int           n_child;
    char          curr_dir[100];
    char          temp_curr_dir[100];
    char          echo_var[5][100];
    int           echo_count;
    pcb_t        *next_proc;
    mm_struct_t  *mm;
    register_t    regs;
}__attribute__((packed));


extern int get_next_processID();
extern void create_kernel_thread();
extern void create_new_thread();
extern pcb_t* create_user_process();
extern void test_function();
extern void test_user_thread();
extern void scheduler();
extern void switchTask(pcb_t *current, pcb_t *next);
extern void switchBack(pcb_t *current, pcb_t *next);
extern void user_switchBack(pcb_t *current);
extern void initial_ret_function();
extern void switch_to_ring3(pcb_t *pcb);
extern void set_user_space(pcb_t *user_process, uint64_t offset);
extern pid_t fork_child();
extern pcb_t* copy_parent_structure(pcb_t *parent_proc);
extern uint64_t get_pid();
extern void wait(uint64_t pid);
extern void execve(char *filename, char *argv);
extern void execve_switch_to_ring3(pcb_t *proc);
extern void print_allPID();
extern void sys_exit();
extern void user_switchTask(pcb_t *current, pcb_t *next);
extern void kill(uint64_t pid);
extern void schedule_sbush();
extern void setEnv(char *s);
extern void printEnv(char *s);
#endif
