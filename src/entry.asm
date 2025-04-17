BITS 64

global entry

extern kernel_main

entry:
    mov rdi, rsp
    jmp kernel_main