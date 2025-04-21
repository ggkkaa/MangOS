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
#include "memory/linked_list.h"
#include "limine/limine.h"
#include "memory/page.h"
<<<<<<< HEAD
=======
#include "drivers/framebuffer.h"
>>>>>>> 4782054 (stuff)

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3)

__attribute__((used, section(".limine_requests")))
<<<<<<< HEAD
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
=======
>>>>>>> 4782054 (stuff)
volatile struct limine_hhdm_request limine_hhdm_request = {
        .id = LIMINE_HHDM_REQUEST,
        .revision = 0,
};

__attribute__((used, section(".limine_requests")))
volatile struct limine_executable_address_request limine_kernel_addr_request = {
    .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
volatile struct limine_stack_size_request limine_stack_size_request = {
    .id = LIMINE_STACK_SIZE_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER

Kernel kernel = {0};

<<<<<<< HEAD
void kernel_main(void* rsp) {
=======
void kernel_main() {
>>>>>>> 4782054 (stuff)
    init_serial();

    if(LIMINE_BASE_REVISION_SUPPORTED == false) {
        kpanic("This limine base revision is not supported.");
        halt();
    }

<<<<<<< HEAD
    kinfo("Limine stack is at %p", rsp);

=======
>>>>>>> 4782054 (stuff)
    kinfo("Disabling interrupts.");
    disable_interrupts();
    kinfo("Initializing GDT");
    init_GDT(); 
    kinfo("GDT Initialized");
    kinfo("Initializing IDT");
    init_IDT(); 
    kinfo("IDT Initialized"); 

    uintptr_t hhdm = limine_hhdm_request.response->offset;
        kernel.phys_addr = limine_kernel_addr_request.response->physical_base;
        kernel.virt_addr = (void*)limine_kernel_addr_request.response->virtual_base;
    kinfo("hhdm offset: %p", hhdm);

<<<<<<< HEAD
    kinfo("Finding framebuffer...");

    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        kpanic("Error! Framebuffer not found!");
        halt();
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    volatile uint32_t *fb_ptr = framebuffer->address;

    kinfo("Framebuffer found!");

    for (size_t i = 0; i < 100; i++) {
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0x3ae3fe;
    }
    

    init_list(hhdm);
    kinfo("PList has been initialized");
    init_paging();

    uint16_t stack_size = (limine_stack_size_request.stack_size < KERNEL_STACK_PAGES * PAGE_SIZE) ? limine_stack_size_request.stack_size : KERNEL_STACK_PAGES;

    asm volatile(\
            "mov %0, %%cr3\n"\
            "mov %1, %%rsp\n"\
            "mov $0, %%rbp\n"\
            :\
            : "r" ((uintptr_t)kernel.pml4 - kernel.hhdm),\
              "r" (KERNEL_STACK_PTR - stack_size)
    );

    memcpy((void*)KERNEL_STACK_ADDR, rsp, stack_size);


    kinfo("We have paging");

=======
    init_list(hhdm);
    kinfo("PList has been initialized");
    init_paging();
    
    asm volatile(\
            "mov %0, %%cr3\n"\
            :\
            : "r" ((uintptr_t)kernel.pml4 - kernel.hhdm)
    );


    kinfo("We have paging");

    init_framebuffer();

    //draw_char('a', 3, 4);

>>>>>>> 4782054 (stuff)
    halt();

}