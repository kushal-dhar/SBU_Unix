#ifndef _PROCESS_H
#define _PROCESS_H

#define MAX_PROCESS_COUNT      64
#define MAX_FD_COUNT           100

// Task States
#define  TASK_NEW         1
#define  TASK_READY       2
#define  TASK_RUNNING     3
#define  TASK_WAITING     4
#define  TASK_STOPPED     5
#define  TASK_ZOMBIE      6

typedef struct process_ctrl_blk pcb_t;
typedef struct reg_struct mm_struct_t;
typedef struct registers register_t;

struct reg_struct {
    uint64_t  code_start, code_end, data_start, data_end;
    uint64_t  start_brk, stack_start;
    uint64_t  arg_start, arg_end;
    uint64_t  env_start, env_end;
    uint64_t  rss;
    uint64_t  vm_total, vm_locked;
};

struct registers {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp, r8, r9, r10, r11;
};

//PCB for each processes
struct process_ctrl_blk {
    char          kernel_stack[4096];
    int           pid;
    int           ppid;
    char          p_name[30];
    uint64_t      init_kernel;
    uint64_t      rsp;
    uint64_t      rip;
    uint64_t      cr3;
    int           state;
    pcb_t        *next_proc;
    mm_struct_t  *mm;
};


extern int get_next_processID();
extern void create_kernel_thread();
extern void create_new_thread();
extern void test_function();
extern void scheduler();
extern void switchTask(pcb_t *current, pcb_t *next);
extern void switchBack(pcb_t *current, pcb_t *next);
extern void initial_ret_function();

#endif
