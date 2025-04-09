; MIT License
;
; Copyright (c) 2024 ggkkaa
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all
; copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.


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
global virtualization_exception;
global control_protection_exception;
global hypervisor_injection_exception;
global VMM_communication_exception;
global security_exception;

division_exception:
    push 0
    push 0
    jmp base_handler

debug_exception:
    push 0
    push 1
    jmp base_handler

breakpoint_exception:
    push 0
    push 3
    jmp base_handler

overflow_exception:
    push 0
    push 4
    jmp base_handler

bound_range_exceeded_exception:
    push 0
    push 5
    jmp base_handler

invalid_opcode_exception:
    push 0
    push 6
    jmp base_handler

device_not_available_exception:
    push 0
    push 7
    jmp base_handler

double_fault_exception:
    push 8
    jmp base_handler

coprocessor_segment_exception:
    push 0
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
    push 0
    push 16
    jmp base_handler

alignment_check_exception:
    push 17
    jmp base_handler

machine_check_exception:
    push 0
    push 18
    jmp base_handler

SIMD_floating_point_exception:
    push 0
    push 19
    jmp base_handler

virtualization_exception:
    push 0
    push 20

control_protection_exception:
    push 21

hypervisor_injection_exception:
    push 0
    push 28

VMM_communication_exception:
    push 29

security_exception:
    push 30

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
    mov rax, cr2
    push rax
    mov rdi, rsp
    call error_handler
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp, 0x10
    iretq
