#include "print.h"
#include "strconvert.h"
#include "multiboot2.h"
#include "kernel.h"
#include "utils.h"

const char* tag_type_map[] = {
    [MULTIBOOT_TAG_ALIGN                ] = "MULTIBOOT_TAG_ALIGN",
    [MULTIBOOT_TAG_TYPE_END             ] = "MULTIBOOT_TAG_TYPE_END",
    [MULTIBOOT_TAG_TYPE_CMDLINE         ] = "MULTIBOOT_TAG_TYPE_CMDLINE", 
    [MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME] = "MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME",
    [MULTIBOOT_TAG_TYPE_MODULE          ] = "MULTIBOOT_TAG_TYPE_MODULE",
    [MULTIBOOT_TAG_TYPE_BASIC_MEMINFO   ] = "MULTIBOOT_TAG_TYPE_BASIC_MEMINFO",
    [MULTIBOOT_TAG_TYPE_BOOTDEV         ] = "MULTIBOOT_TAG_TYPE_BOOTDEV", 
    [MULTIBOOT_TAG_TYPE_MMAP            ] = "MULTIBOOT_TAG_TYPE_MMAP",       
    [MULTIBOOT_TAG_TYPE_VBE             ] = "MULTIBOOT_TAG_TYPE_VBE",          
    [MULTIBOOT_TAG_TYPE_FRAMEBUFFER     ] = "MULTIBOOT_TAG_TYPE_FRAMEBUFFER",
    [MULTIBOOT_TAG_TYPE_ELF_SECTIONS    ] = "MULTIBOOT_TAG_TYPE_ELF_SECTIONS",   
    [MULTIBOOT_TAG_TYPE_APM             ] = "MULTIBOOT_TAG_TYPE_APM",  
    [MULTIBOOT_TAG_TYPE_EFI32           ] = "MULTIBOOT_TAG_TYPE_EFI32",           
    [MULTIBOOT_TAG_TYPE_EFI64           ] = "MULTIBOOT_TAG_TYPE_EFI64",           
    [MULTIBOOT_TAG_TYPE_SMBIOS          ] = "MULTIBOOT_TAG_TYPE_SMBIOS",          
    [MULTIBOOT_TAG_TYPE_ACPI_OLD        ] = "MULTIBOOT_TAG_TYPE_ACPI_OLD",        
    [MULTIBOOT_TAG_TYPE_ACPI_NEW        ] = "MULTIBOOT_TAG_TYPE_ACPI_NEW",        
    [MULTIBOOT_TAG_TYPE_NETWORK         ] = "MULTIBOOT_TAG_TYPE_NETWORK",         
    [MULTIBOOT_TAG_TYPE_EFI_MMAP        ] = "MULTIBOOT_TAG_TYPE_EFI_MMAP",        
    [MULTIBOOT_TAG_TYPE_EFI_BS          ] = "MULTIBOOT_TAG_TYPE_EFI_BS",          
    [MULTIBOOT_TAG_TYPE_EFI32_IH        ] = "MULTIBOOT_TAG_TYPE_EFI32_IH",        
    [MULTIBOOT_TAG_TYPE_EFI64_IH        ] = "MULTIBOOT_TAG_TYPE_EFI64_IH",        
    [MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR  ] = "MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR",
};

Kernel kernel = {0};

void kernel_main(uint32_t magic, uint32_t addr) {
    init_serial();

    char buf[20];
    const char* hex_upper_digits = "0123456789ABCDEF";
    
    struct multiboot_tag* tag = (struct multiboot_tag*)(addr+8);
    while(tag->type != MULTIBOOT_TAG_TYPE_END) {
        serial_write_str("Found tag: "); 
        if(tag->type < sizeof(tag_type_map)/sizeof(tag_type_map[0])) 
            serial_write_str(tag_type_map[tag->type]);
        else 
            serial_write_str("Unknown");
        serial_write_str("\r\n");
        if(tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) { //Check the current tag
            struct multiboot_tag_framebuffer* tagfb = (struct multiboot_tag_framebuffer*) tag;
            uint32_t* fb = (uint32_t*)((long int)tagfb->common.framebuffer_addr);

            if(tagfb->common.framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
                serial_write_str("The framebuffer isn't rgb");
                for(;;) asm volatile("hlt");
            }
            if(tagfb->common.framebuffer_bpp != 32) {
                serial_write_str("We have a different amount of bits per pixel than 32");
                for(;;) asm volatile("hlt");
            }
            if(tagfb->common.framebuffer_addr == 0) {
                serial_write_str("Addr is NULL");
            }
            buf[uptrtoha_full(buf, sizeof(buf), tagfb->common.framebuffer_addr, hex_upper_digits)] = '\0';
            serial_write_str("This is framebuffer_addr: ");
            serial_write_str(buf);
            serial_write_str("\r\n");
            //fb[0] = (uintptr_t)0xffffffff;
//            for(int y = 0; y < tagfb->common.framebuffer_height; ++y) {
//                for(int x = 0; x < tagfb->common.framebuffer_width; ++x) {
//                    uint32_t* row = (uint32_t*)(((uint8_t*)fb)+tagfb->common.framebuffer_pitch*y);
//                    row[x] = 0xff0000;
//                }
//            } 
        }
        tag = (struct multiboot_tag *) (((uint8_t*)tag) + ((tag->size + 7) & ~7));
    }

    printf("This is an integer: %d\n", 69);
    printf("This is a pointer : %p\n", (void*)0xabcd);
    printf("This is a string  : %s\n", "Hello!");
    printf("Disabling interrupts.\n");
    disable_interrupts();
    printf("Initializing GDT\n");
    init_GDT(); 
    printf("GDT Initialized\n");
    printf("Initializing IDT\n");
    init_IDT(); 
    printf("IDT Initialized\n");
    printf("%d\n", 0/0);
}