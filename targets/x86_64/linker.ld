    ENTRY(start)

    SECTIONS
    {
        . = 1M;

        .boot :
        {
            KEEP(*(.multiboot_header))
        }

        .text :
        {
            kernel_start = .;
            *(.text)
            kernel_end = .;
        }
    }