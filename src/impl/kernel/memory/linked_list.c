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

#include "./linked_list.h"
#include "../kernel.h"
#include "print.h"
#include "../panic.h"
#include "limine/limine.h"
#include "bootutils.h"

extern struct limine_hhdm_request limine_hhdm_request;

/*
    Initializes the lined list for physical allocation.

    First, it initializes the list and it reads the mmap tags.

    Then, it prints them out and assigns a list node at the
    beginning of each

*/

void init_list(uintptr_t hhdm_offset) {
        kllog("hhdm offset: %p", 1, 0, hhdm_offset);
        kernel.hhdm = hhdm_offset;

        kernel.available_pages = 0;
        list_init(&kernel.memory_list.list);

        BootMemRegion region;
        
        kllog("%d", 1, 0, boot_get_memregion_count());

        for (size_t i = 0; i < boot_get_memregion_count(); ++i)
        {
            boot_get_memregion_at(&region, i);
            kllog("Region %d: addr = %p size = %p kind = %d", 1, 0, (int)i, (void*)region.address, (void*)(uintptr_t)region.size, (int)region.kind);

            if(region.kind == BOOT_MEMREGION_USABLE) {
                if(region.address < BOOT_HHDM_SIZE) {
                    paddr_t region_end = region.address + region.size;
                    size_t pages_available = region.size/PAGE_SIZE;
                    if(region_end > BOOT_HHDM_SIZE) {
                        pages_available = (BOOT_HHDM_SIZE - region.address) / PAGE_SIZE;
                    }
                    void* region_virtual = (void*)(region.address + kernel.hhdm);
                    struct list_node* node = region_virtual;
                    list_init(&node->list);
                    node->pages = pages_available - 1;
                    list_append(&node->list, &kernel.memory_list.list);
                } else {
                    kllog("Region available but ignored.", 1, 0);
                }
            }
        }  

        kllog("Finished setting up the list. Now starting test.", 1, 0);

        allocator_test();
}

// Allocates a single physical page.

paddr_t alloc_phys_page() {
    if(list_empty(&kernel.memory_list.list)) return (paddr_t)NULL;
    struct list_node* node = (struct list_node*)kernel.memory_list.list.next;
    void* result = (void*)node;

    if(node->pages) {
        struct list_node* new_node = (struct list_node*)(((char*)node) + PAGE_SIZE);
        new_node->pages = node->pages - 1;
        list_init(&new_node->list);
        list_append(&new_node->list, &node->list);
    }
    if(node->list.next == NULL) kpanic("Hey. It was NULL");
    kllog("alloc: node->list.next is at %p", 1, 0, &node->list.next);
    kllog("alloc: node->list.next is %p", 1, 0, node->list.next);
    list_remove(&node->list);
    kllog("alloc: phys page is at %p", 1, 0, result);
    return (paddr_t)(result - limine_hhdm_request.response->offset);
}

// Allocates multiple physical pages

paddr_t alloc_phys_pages(size_t pages_count) {
    if(pages_count == 0) return (paddr_t)NULL;
    if(pages_count == 1) return alloc_phys_page();
    pages_count--;

    for(struct list* list = kernel.memory_list.list.next; list != &kernel.memory_list.list; list = list->next) {
        struct list_node* node = (struct list_node*)list;
        void* result = node;
        if(node->pages > pages_count) {
            struct list_node* new_node = (struct list_node*)(((char*)node) + PAGE_SIZE*pages_count);
            new_node->pages = node->pages - pages_count;
            list_init(&new_node->list);
            list_append(&new_node->list, list);
            list_remove(list);
            kllog("alloc: phys page is at %p", 1, 0, result);
            return (paddr_t)result - limine_hhdm_request.response->offset;
        } else if(node->pages == pages_count) {
            list_remove(list);
            return (paddr_t)result - limine_hhdm_request.response->offset;
        }
    }
    return (paddr_t)NULL;
}

void free_phys_pages(paddr_t page, size_t count) {
    struct list_node* node = (struct list_node*)(page + limine_hhdm_request.response->offset);
    list_init(&node->list);
    node->pages = count - 1;
    list_append(&node->list, &kernel.memory_list.list);
}

#define TEST_ALLOC_SIZE 2

/* Tests the physical allocator. Has two parts:

    1: Allocates a single page, loops over it, 
    and checks all the bytes for a pattern.

    2: Allocates multiple pages(amount of which
    is defined in TEST_ALLOC_SIZE), and checks 
    all of them for a pattern.

*/

void allocator_test() {
        bool success = true;
        uint8_t* test_int = (uint8_t*)((long long unsigned int)alloc_phys_pages(1) | KERNEL_MEMORY_MASK);
        if(!test_int) kpanic("Test int is null.");
        kllog("Address of the test int is %p", 1, 0, test_int);
    *test_int = 69;
    
    kllog("testing integer, should be 69: %d", 1, 0, *test_int);

    for (size_t i = 0; i < PAGE_SIZE; i++) {
        test_int[i] = (uint8_t)(i & 0xFF);
    }

    kllog("Pattern filled.", 1, 0);

    for (size_t i = 0; i < PAGE_SIZE; i++) {
        if (test_int[i] != (uint16_t)(i & 0xFF)) {
            success = false;
            kllog("Memory test failed at byte %p: Expected %d, got %d", 1, 4, i + test_int, (uint16_t)(i & 0xFF), test_int[i]);
            break;
        }
    }

    free_phys_pages((paddr_t)(test_int - limine_hhdm_request.response->offset), 1);

    if(success == false) {
        kllog("Error! Memory allocation test failed!!!", 1, 2);
        kpanic("Single Page Allocation Failed");
    } else {
        kllog("Single page physical allocation test finished.", 1, 0);
    }

    kllog("Multi page physical allocation test starting...", 1, 0);

    test_int = (uint8_t*)((long long unsigned int)alloc_phys_pages(TEST_ALLOC_SIZE) | KERNEL_MEMORY_MASK);

    for (size_t i = 0; i < TEST_ALLOC_SIZE; i++) {
        test_int[i] = (uint16_t)(i & 0xFF);
        
    }


    for (size_t i = 0; i < TEST_ALLOC_SIZE; i++) {
        if (test_int[i] != (uint16_t)(i & 0xFF)) {
            kllog("Multi-page test failed you're dumb", 1, 2);
            return;
        }
    }

    free_phys_pages((paddr_t)(test_int - limine_hhdm_request.response->offset), TEST_ALLOC_SIZE);

    kllog("test passed!", 1, 0);

}