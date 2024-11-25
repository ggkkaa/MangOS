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

    kllog("This is an integer: %d", 1, 0, 69);
    kllog("This is a pointer : %p", 1, 0, (void*)0xabcd);
    kllog("This is a string  : %s", 1, 0, "Hello!");
    kllog("Disabling interrupts.", 1, 0);
    disable_interrupts();
    kllog("Initializing GDT", 1, 0);
    init_GDT(); 
    kllog("GDT Initialized", 1, 0);
    kllog("Initializing IDT", 1, 0);
    init_IDT(); 
    kllog("IDT Initialized", 1, 0);

    kllog("Reading multiboot address.", 1, 0);
    struct multiboot_tag* tag = (struct multiboot_tag*)(addr+8);
    while(tag->type != MULTIBOOT_TAG_TYPE_END) {
        kllog("Found tag: ", 0, 0); 
        if(tag->type < sizeof(tag_type_map)/sizeof(tag_type_map[0])) 
            kllog(tag_type_map[tag->type], 1, 200);
        else 
            kllog("Unknown", 1, 1);
        if(tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) { //Check the current tag
            struct multiboot_tag_framebuffer* tagfb = (struct multiboot_tag_framebuffer*) tag;
            uint32_t* fb = (uint32_t*)((long int)tagfb->common.framebuffer_addr);

            if(tagfb->common.framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
                kllog("The framebuffer isn't rgb", 1, 2);
                for(;;) asm volatile("hlt");
            }
            if(tagfb->common.framebuffer_bpp != 32) {
                kllog("We have a different amount of bits per pixel than 32", 1, 2);
                for(;;) asm volatile("hlt");
            }
            if(tagfb->common.framebuffer_addr == 0) {
                kllog("Addr is NULL", 1, 2);
            }
            buf[uptrtoha_full(buf, sizeof(buf), tagfb->common.framebuffer_addr, hex_upper_digits)] = '\0';
            kllog("This is framebuffer_addr: %p", 1, 0, buf);
            //fb[0] = (uintptr_t)0xffffffff;
        } else if(tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            struct multiboot_tag_mmap* tagmmap = (struct multiboot_tag_mmap*) tag;
            
            struct multiboot_mmap_entry* mmap_entry = tagmmap->entries;
            while ((uint8_t*) mmap_entry < (uint8_t*) tagmmap + tagmmap->size) {
                uint64_t base_address = mmap_entry->addr;
                uint64_t length = mmap_entry->len;
                uint32_t type = mmap_entry->type;

                kllog("Memory address at %p found", 1, 0, base_address);
                kllog("Length of memory address is %d", 1, 0, length);

                if (type == MULTIBOOT_MEMORY_AVAILABLE) {
                    kllog("Type: Available", 1, 0);
                } else {
                    kllog("Type: Reserved/Other", 1, 0);
                }

                mmap_entry = (struct multiboot_mmap_entry*)((uint8_t*) mmap_entry + tagmmap->entry_size);
            }
        }
        tag = (struct multiboot_tag *) (((uint8_t*)tag) + ((tag->size + 7) & ~7));
    }

}