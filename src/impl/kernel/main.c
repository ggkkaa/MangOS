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
#include "panic.h"
#include "strconvert.h"
#include "kernel.h"
#include "utils.h"
#include "./memory/linked_list.h"
#include "limine/limine.h"
#include "page.h"

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
volatile struct limine_hhdm_request limine_hhdm_request = {
        .id = LIMINE_HHDM_REQUEST,
        .revision = 0,
};

__attribute__((used, section(".limine_requests")))
volatile struct limine_executable_address_request limine_kernel_addr_request = {
    .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

/*
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
};*/

Kernel kernel = {0};

void kernel_main() {
    init_serial();

    if(LIMINE_BASE_REVISION_SUPPORTED == false) {
        kpanic("This limine base revision is not supported.");
        halt();
    }

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

    uintptr_t hhdm = limine_hhdm_request.response->offset;
        kernel.phys_addr = limine_kernel_addr_request.response->physical_base;
        kernel.virt_addr = (void*)limine_kernel_addr_request.response->virtual_base;
    kllog("hhdm offset: %p", 1, 0, hhdm);

    kllog("Finding framebuffer...", 1, 0);

    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        kpanic("Error! Framebuffer not found!");
        halt();
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    volatile uint32_t *fb_ptr = framebuffer->address;

    kllog("Framebuffer found!", 1, 0);

    for (size_t i = 0; i < 100; i++) {
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0x3ae3fe;
    }
    

    init_list(hhdm);
    kllog("PList has been initialized", 1, 0);
    init_paging();

    halt();

}