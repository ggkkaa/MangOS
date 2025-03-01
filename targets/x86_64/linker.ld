    ENTRY(start)

    SECTIONS
    {
        . = 1M;

        kernel_start = .;

        .boot :
        {
            KEEP(*(.multiboot_header))
        }

        .text :
        {
            *(.text)
        }
        kernel_end = .;
    }