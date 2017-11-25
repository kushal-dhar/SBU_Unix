.global isr32
.global isr33
.global isr14
.global isr128
.extern intr_handler

isr32:
    movq $32, %rax
    jmp common_handler
isr33:
    movq $33, %rax
    jmp common_handler
isr14:
    movq $14, %rax
    jmp common_handler
isr128:
    movq $128, %rax
    jmp common_handler

common_handler:
/*    pushq %rax */
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11 
    call intr_handler
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
/*    popq %rax */
    iretq

