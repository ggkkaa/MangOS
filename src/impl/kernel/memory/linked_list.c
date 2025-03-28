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

__attribute__((used, section(".limine_requests")))
volatile struct limine_memmap_request limine_memmap_request = {
        .id = LIMINE_MEMMAP_REQUEST,
        .revision = 0,
};
/*
    Initializes the lined list for physical allocation.

    First, it initializes the list and it reads the mmap tags.

    Then, it prints them out and assigns a list node at the
    beginning of each

*/

static const char* limine_memmap_str[] = {
        "Usable",
        "Reserved",
        "Acpi Reclaimable",
        "Acpi NVS",
        "Bad Memory",
        "Bootloader Reclaimable",
        "Kernel and Modules",
        "Framebuffer"
};

void init_list(uintptr_t hhdm_offset) {
        kllog("hhdm offset: %p", 1, 0, hhdm_offset);

        kernel.available_pages = 0;
        list_init(&kernel.memory_list.list);
        uint64_t available_memory = 0;

        kernel.memmap_request = limine_memmap_request;

        kllog("entries array is at: %p", 1, 0, kernel.memmap_request.response->entries);
        for (size_t i = 0; i < kernel.memmap_request.response->entry_count; ++i) {
            kllog("entry[%d] = %p", 1, 0, i, kernel.memmap_request.response->entries[i]);
        }
        

        for (size_t i = 0; i < limine_memmap_request.response->entry_count; ++i)
        {
                struct limine_memmap_entry* entry = limine_memmap_request.response->entries[i];

                kllog("The entry header is at %p", 1, 0, entry);

                kllog("%d st memory entry at %p", 1, 0, i, (void*)entry->base);

                kllog("%d pages.", 1, 0, (size_t)(entry->length / PAGE_SIZE));

                kllog("Type: %s", 1, 0, limine_memmap_str[entry->type]);



                if (entry->base > 4ULL * 1024 * 1024 * 1024) {
                    kllog("The entry is above 4 GB", 1, 0);
                    continue;
                }
            

                if(entry->type == LIMINE_MEMMAP_USABLE) {
                        // set up a node for the free spot in memory,

                        if(entry->base >= PHYS_MEM_RESERVE_SIZE) {
                            kllog("Skipping entry, base out of range %p", 1, 0, entry->base);
                            continue;
                        }
                        
                        if (entry->base + entry->length > PHYS_MEM_RESERVE_SIZE) {
                            kllog("Clamping entry. Entry was too long.", 1, 0);
                            entry->length = PHYS_MEM_RESERVE_SIZE - entry->base;
                        }

                        struct list_node *virtual_node_loc = (struct list_node*)(entry->base + hhdm_offset);

                        struct list_node *current_node = virtual_node_loc;

                        list_init(&current_node->list);
                        
                        current_node->pages = entry->length / PAGE_SIZE;
                        
                        list_append(&current_node->list, &kernel.memory_list.list);
                        
                        // print out the node that was set up
                        kllog("The memory setup node is at: %p", 1, 0, current_node);
                        kllog("The memory setup node is %d pages big", 1, 0, current_node->pages);
                        
                        // And modify kernel values.
                        available_memory += entry->length;
                        kernel.available_pages = available_memory / PAGE_SIZE;
                } else if(entry->type == LIMINE_MEMMAP_EXECUTABLE_AND_MODULES) {
                    struct list_node *virtual_node_loc = (struct list_node*)(entry->base + hhdm_offset);
                    kllog("init_list: Kernel entry at %p", 1, 0, virtual_node_loc);
                }
        }
        
        if(kernel.memory_list.list.next == &kernel.memory_list.list)
        {
                kpanic("The list is empty.");
        }

        kllog("The main node is at %p", 1, 0, &kernel.memory_list.list);

        for(struct list* list = kernel.memory_list.list.next; list != &kernel.memory_list.list; list = list->next) {
                kllog("This node is at %p", 1, 0, list);
            }

        kllog("Finished setting up the list. Now starting test.", 1, 0);

        allocator_test();
}

// Allocates a single physical page.

void* alloc_phys_page() {
    struct list_node* node = (struct list_node*)kernel.memory_list.list.next;

    void* result = (void*)node;
    if(node->pages > 0) {
        struct list_node* new_node = (struct list_node*)((char*)node + PAGE_SIZE);
        new_node->pages = node->pages - 1;
        list_init(&new_node->list);
        list_append(&new_node->list, &node->list);
    }
    list_remove(&node->list);
    kllog("alloc: phys page is at %p", 1, 0, result);
    return result;
}

// Allocates multiple physical pages

void* alloc_phys_pages(size_t pages_count) {
    size_t size = 0;
    for (struct list* i = kernel.memory_list.list.next; i != &kernel.memory_list.list; i = i->next) {
        kllog("entry[%d] = %p", 1, 0, size, i);
        size++;
    }
        if(pages_count == 1) {
                void* result = alloc_phys_page();
                return result;
        }
    for(struct list* list = kernel.memory_list.list.next; list != &kernel.memory_list.list; list = list->next) {
        struct list_node* node = (struct list_node*)list;
        void* result = node;
        if(node->pages > pages_count) {
            struct list_node* new_node = (struct list_node*)((char*)node + PAGE_SIZE*pages_count);
            new_node->pages = node->pages - pages_count;
            list_init(&new_node->list);
            list_append(&new_node->list, &node->list);
            list_remove(&node->list);
            kllog("alloc: phys page is at %p", 1, 0, result);
            return result;
        } else if(node->pages) {
            list_remove(list);
            kllog("alloc: phys page is at %p", 1, 0, result);
            return result;
        }
    }
    return NULL;
}

void free_phys_pages(void* page, size_t count) {
    struct list_node* node = (struct list_node*)page;
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
        uint8_t* test_int = (uint8_t*)alloc_phys_pages(1);
        kllog("Address of the test int is %p", 1, 0, test_int);
    *test_int = 69;
    
    kllog("testing integer, should be 69: %d", 1, 0, *test_int);

    for (size_t i = 0; i < PAGE_SIZE; i++) {
        if(i == 251) {
            //continue;
        }
        test_int[i] = (uint8_t)(i & 0xFF);
        
        //kllog("Write %d. %d Written.", 1, 0, i, test_int[i]);
    }

    kllog("Pattern filled.", 1, 0);

    for (size_t i = 0; i < PAGE_SIZE; i++) {
        if (test_int[i] != (uint16_t)(i & 0xFF)) {
            success = false;
            kllog("Memory test failed at byte %p: Expected %d, got %d", 1, 4, i + test_int, (uint16_t)(i & 0xFF), test_int[i]);
            break;
        }
    }

    free_phys_pages(test_int, 1);

    if(success == false) {
        kllog("Error! Memory allocation test failed!!!", 1, 2);
        kpanic("Single Page Allocation Failed");
    } else {
        kllog("Single page physical allocation test finished.", 1, 0);
    }

    kllog("Multi page physical allocation test starting...", 1, 0);

    test_int = (uint8_t*)alloc_phys_pages(TEST_ALLOC_SIZE);

    for (size_t i = 0; i < TEST_ALLOC_SIZE; i++) {
        test_int[i] = (uint16_t)(i & 0xFF);
        
    }


    for (size_t i = 0; i < TEST_ALLOC_SIZE; i++) {
        if (test_int[i] != (uint16_t)(i & 0xFF)) {
            kllog("Multi-page test failed you're dumb", 1, 2);
            return;
        }
    }

    free_phys_pages(test_int, TEST_ALLOC_SIZE);

    kllog("test passed!", 1, 0);

}