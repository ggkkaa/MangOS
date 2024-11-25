section .text
bits 64

extern error_handler

global division_exception
global debug_exception;
global breakpoint_exception;
global overflow_exception;
global bound_range_exceeded_exception;
global invalid_opcode_exception;
global device_not_available_exception;
global double_fault_exception;
global coprocessor_segment_exception;
global invalid_TSS_exception;
global segment_not_present_exception;
global stack_segment_exception;
global general_protection_exception;
global page_fault_exception;
global floating_point_exception;
global alignment_check_exception;
global machine_check_exception;
global SIMD_floating_point_exception;

division_exception:
    push 0
    jmp base_handler

debug_exception:
    push 1
    jmp base_handler

breakpoint_exception:
    push 3
    jmp base_handler

overflow_exception:
    push 4
    jmp base_handler

bound_range_exceeded_exception:
    push 5
    jmp base_handler

invalid_opcode_exception:
    push 6
    jmp base_handler

device_not_available_exception:
    push 7
    jmp base_handler

double_fault_exception:
    push 8
    jmp base_handler

coprocessor_segment_exception:
    push 9
    jmp base_handler

invalid_TSS_exception:
    push 10
    jmp base_handler

segment_not_present_exception:
    push 11
    jmp base_handler

stack_segment_exception:
    push 12
    jmp base_handler

general_protection_exception:
    push 13
    jmp base_handler

page_fault_exception:
    push 14
    jmp base_handler

floating_point_exception:
    push 16
    jmp base_handler

alignment_check_exception:
    push 17
    jmp base_handler

machine_check_exception:
    push 18
    jmp base_handler

SIMD_floating_point_exception:
    push 19
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
    mov rax, rsp
    add rax, 120
    mov rdi, [rax]
    call error_handler
