section .text
bits 64

extern error_handler

global division_exception

division_exception:
    push 0
    jmp base_handler

base_handler:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    mov rdi, rax
    call error_handler
