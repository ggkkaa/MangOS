BITS 64

global entry

extern kernel_main

entry:
    mov rsp, stack_top
    jmp kernel_main

section .bss
align 16
stack_bottom:
    resb 40960
global stack_top
stack_top:
