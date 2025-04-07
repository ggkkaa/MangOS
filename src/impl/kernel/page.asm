section .text
    global load_page_dir
    global enable_paging

load_page_dir:
    push rbp
    mov rdi, [rsp + 8]       ; Load the first argument (pageDirectory) into rdi
    mov cr3, rdi             ; Set CR3 to the value in rdi (page directory)
    pop rbp
    ret

enable_paging:
    push rbp
    mov rax, cr0             ; Load CR0 into rax
    or rax, 0x80000000       ; Set the PG bit (bit 31) in CR0 to enable paging
    mov cr0, rax             ; Write the modified value back to CR0
    pop rbp
    ret
