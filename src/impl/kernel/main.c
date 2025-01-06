/* MIT License
*
* Copyright (c) 2024 ggkkaa
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "print.h"
#include "strconvert.h"
#include "multiboot2.h"
#include "kernel.h"
#include "utils.h"
#include "./memory/linked_list.h"


const char* tag_type_map[] = {
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

void kernel_main(uint32_t magic, uintptr_t addr) {
    init_serial();

    uintptr_t start = (uintptr_t)&kernel_start;
    uintptr_t end = (uintptr_t)&kernel_end;
    uintptr_t size = end - start;

    char buf[20];
    const char* hex_upper_digits = "0123456789ABCDEF";

    kllog("This is an integer: %d", 1, 0, 69);
    kllog("This is a pointer : %p", 1, 0, (void*)0xabcd);
    kllog("This is a string  : %s", 1, 0, "Hello!");
    kllog("Multiple variable test %p, %d, %s, %p", 1, 0, (void*)0xabcd, 69, "Test", (void*)0xef12);
    kllog("Disabling interrupts.", 1, 0);
    disable_interrupts();
    kllog("Initializing GDT", 1, 0);
    init_GDT(); 
    kllog("GDT Initialized", 1, 0);
    kllog("Initializing IDT", 1, 0);
    init_IDT(); 
    kllog("IDT Initialized", 1, 0);

    kllog("Kernel Start: %p", 1, 0, start);
    kllog("Kernel End: %p", 1, 0, end);
    kllog("Kernel Size: %d", 1, 0, size);

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
            fb[0] = (uintptr_t)0xffffffff;
        } else if(tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
            kllog("Initializing memory", 1, 0);
            init_list(tag);
            allocator_test();
        }
        tag = (struct multiboot_tag *) (((uint8_t*)tag) + ((tag->size + 7) & ~7));
    }


}