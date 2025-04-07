OUTPUT_FORMAT(elf64-x86-64)

ENTRY(kernel_main)

PHDRS
{
        limine_requests PT_LOAD;
        text PT_LOAD;
        rodata PT_LOAD;
        data PT_LOAD;
}

SECTIONS
{
        . = 0xffffffff80000000;
        .limine_requests : {
                KEEP(*(.limine_requests_start))
                KEEP(*(.limine_requests))
                KEEP(*(.limine_requests_end))
        } :limine_requests

        . = ALIGN(CONSTANT(MAXPAGESIZE));

        KERNEL_START = .;

        section_text_begin = .;
        .text : {
                *(.text .text.*)
        } :text
        section_text_end = .;

        . = ALIGN(CONSTANT(MAXPAGESIZE));

        .rodata : {
                *(.rodata .rodata.*)
        } :rodata

        . = ALIGN(CONSTANT(MAXPAGESIZE));

        WRITE_ALLOWED_START = .;

        .data : {
                *(.data .data.*)
        } :data 

        .bss : {
                *(.bss .bss.*)
                *(COMMON)
        } :data

        KERNEL_END = .;

        /DISCARD/ : {
                *(.eh_frame*)
                *(.note .note.*)
        }
}