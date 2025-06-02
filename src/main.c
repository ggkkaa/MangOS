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
#include "drivers/framebuffer.h"
#include "memory/slab.h"
#include "drivers/fs/vfs.h"

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3)

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

void kernel_main(void* rsp) {
    init_serial();

    if(LIMINE_BASE_REVISION_SUPPORTED == false) {
        kpanic("This limine base revision is not supported.");
        halt();
    }

    kinfo("Limine stack is at %p", rsp);

    kinfo("Disabling interrupts.");
    disable_interrupts();
    kinfo("Initializing GDT");
    init_GDT(); 
    kinfo("GDT Initialized");
    kinfo("Initializing TSS");
    init_TSS();
    kinfo("TSS Initialized");
    kinfo("Initializing IDT");
    init_IDT(); 
    kinfo("IDT Initialized"); 

    uintptr_t hhdm = limine_hhdm_request.response->offset;
        kernel.phys_addr = limine_kernel_addr_request.response->physical_base;
        kernel.virt_addr = (void*)limine_kernel_addr_request.response->virtual_base;
    kinfo("hhdm offset: %p", hhdm);

    init_list(hhdm);
    kinfo("PList has been initialized");
    init_paging();
    
    asm volatile(\
            "mov %0, %%cr3\n"\
            :\
            : "r" ((uintptr_t)kernel.pml4 - kernel.hhdm)
    );


    kinfo("We have paging");

    kinfo("Initializing slab allocator.");
    init_slab_allocator();
    kinfo("Finished initializing.");

    Cache* test = cache_create(4096, "TEST_CACHE");
    void* test_page = cache_alloc(test);
    *(uint32_t*)test_page = 69;
    kinfo("Allocated a page from the slab cache: %p, value %d", test_page, *(uint32_t*)test_page);

    init_framebuffer();

        kinfo("Framebuffer has been initialized");
        kinfo("Framebuffer is at %p", kernel.framebuffer.addr);
        
        kinfo("Initiating VFS");
        init_vfs();
        kinfo("VFS Initialized");

        Inode* path = NULL;
        intptr_t result;

        result = vfs_find_abs("root:/home/", &path);
        if (result < 0) {
        kinfo("Path doesn't exist (This is good, because we didn't create it yet)");
        }

        result = vfs_find_abs("root:/", &path);
        if (result < 0) {
                kwarning("Root does not exist. My find function doesn't work :(");
        halt();
        } else {
                kinfo("Root has been found. My function works!!!");
        }

        kinfo("Creating home...");
        vfs_create_abs("root:/home", INODE_DIR);

        kinfo("Directory creation test.");
        result = vfs_find_abs("root:/home", &path);
        if (result < 0) {
                kwarning("Path doesn't exist even though we just created it!!!");
        halt();
        } else {
                kinfo("Found root:/home/. Folder creation works!!!");
        }

        vfs_create_abs("root:/home/lol.txt", INODE_FILE);

        Inode* inode = NULL;
        result = vfs_find_abs("root:/home/lol.txt", &inode);
        if (result < 0) {
                kwarning("File not found: root:/home/lol.txt");
                halt();
        }

        const char* str = "I CAN FINALLY MAKE FILES YAY";
        const void* ptr = (const void*)str;
        size_t size = 29;

        inode_write(inode, ptr, size, 0);

        char lol[29];
        inode_read(inode, lol, 29, 0);

        kinfo("%s", lol);

        DirEntry direntry[5];

        vfs_create_abs("root:/test", INODE_DIR);
        vfs_create_abs("root:/test2", INODE_DIR);
        vfs_create_abs("root:/test3", INODE_DIR);
        vfs_create_abs("root:/test4", INODE_DIR);

        vfs_find_abs("root:/", &inode);

        inode_get_dir_entries(inode, direntry, 5);

        for (size_t i = 0; i < 5; i++)
        {
                kinfo("%s", direntry[i].name);
        }
        

        halt();
}