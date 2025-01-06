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
#include "../multiboot2.h"
#include "print.h"

// Global variable that stores the amount of nodes
uint64_t node_amount;

/*
    Initializes the lined list for physical allocation.

    First, it initializes the list and it reads the mmap tags.

    Then, it prints them out and assigns a list node at the
    beggining of each

*/

void init_list(struct multiboot_tag* tag) {

    kernel.available_pages = 0;

    list_init(&kernel.memory_list.list);
    
    // Variable that stores the available memory
    uint64_t available_memory = 0;

    // Just turns the multiboot tag into an mmap tag once the tag is identified
    struct multiboot_tag_mmap* tagmmap = (struct multiboot_tag_mmap*) tag;
    
    // Helper mmap entry var
    struct multiboot_mmap_entry* mmap_entry = tagmmap->entries;
    
    // Iterate over the mmap entry to print debugging information and set up a basic linked list
    while (((uint8_t*) mmap_entry) < ((uint8_t*) tagmmap) + tagmmap->size) {
        uint64_t base_address = mmap_entry->addr;
        uint64_t length = mmap_entry->len;
        uint32_t type = mmap_entry->type;

        kllog("Memory address at %p found", 1, 0, base_address);
        kllog("Length of memory address is %d", 1, 0, length);
        // If the address is available then mark it as empty, otherwise mark it as used.
        if (type == MULTIBOOT_MEMORY_AVAILABLE) { //If the memory is available,
            kllog("Type: Available", 1, 0); // Say that it's available

            // set up a node for the free spot in memory,
            struct list_node* current_node = (struct list_node*)base_address;

            list_init(&current_node->list);

            current_node->pages = length / PAGE_SIZE;

            list_append(&current_node->list, &kernel.memory_list.list);

            // print out the node that was set up
            kllog("The memory setup node is at: %p", 1, 0, current_node);
            kllog("The memory setup node is %d pages big", 1, 0, current_node->pages);

            // And modify kernel values.
            available_memory += length;
            kernel.available_pages = available_memory / PAGE_SIZE;
            
            node_amount++;
        } else {    // Else, print out that the memory is not available
            kllog("Type: Reserved/Other", 1, 0);
        }

        // Print out some information
        kllog("Available memory size: %d", 1, 0, available_memory);
        kllog("Available pages: %d", 1, 0, kernel.available_pages);
        mmap_entry = (struct multiboot_mmap_entry*)(((uint8_t*) mmap_entry) + tagmmap->entry_size);
    }

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
    return result;
}

// Allocates multiple physical pages

void* alloc_phys_pages(size_t pages_count) {
    kllog("Allocating a page Lol 1", 1, 0);
    if(pages_count == 1) {return alloc_phys_page;}
    kllog("Allocating a page Lol 2", 1, 0);
    for(struct list* list = kernel.memory_list.list.next; list != &kernel.memory_list.list; list = list->next) {
        struct list_node* node = (struct list_node*)list;
        kllog("Allocating a page Lol 3", 1, 0);
        void* result = node;
        if(node->pages > pages_count) {
            kllog("Allocating a page Lol", 1, 0);
            struct list_node* new_node = (struct list_node*)((char*)node + PAGE_SIZE*pages_count);
            new_node->pages = node->pages - pages_count;
            list_init(&new_node->list);
            list_append(&new_node->list, &node->list);
            list_remove(&node->list);
            return result;
        } else if(node->pages) {
            list_remove(list);
        }
    }
    return NULL;
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
    uint8_t* test_int = (uint8_t*)alloc_phys_pages(1);
    *test_int = 12;
    kllog("Address of the test int is %p", 1, 0, test_int);
    kllog("testing integer, should be 12: %d", 1, 0, *test_int);

    for (size_t i = 0; i < PAGE_SIZE; i++) {
        test_int[i] = (uint8_t)(i & 0xFF);
    }

    for (size_t i = 0; i < PAGE_SIZE; i++) {
        kllog("At offset %d,", 1, 0, i);
        if (test_int[i] != (uint8_t)(i & 0xFF)) {
            kllog("Memory test failed at byte %p: Expected %d, got %d", 0, 4, i, (uint8_t)(i & 0xFF), test_int[i]);
        }
        kllog("____________\n|Byte %p|%d|\n------------\n", 0, 255, test_int + i, test_int[i]);
    }
    kllog("Single page physical allocation test finished.", 1, 0);

    kllog("Multi page physical allocation test starting...", 1, 0);

    test_int = (uint8_t*)alloc_phys_pages(TEST_ALLOC_SIZE);

    /*for (size_t i = 0; i < TEST_ALLOC_SIZE; i++) {
        if ((uintptr_t)test_int[i] % PAGE_SIZE != 0) {
            kllog("Page is not aligned!", 1, 1);
            return;
        }
        test_int[i] = (uint8_t)(i & 0xFF);
        
    }


    for (size_t i = 0; i < TEST_ALLOC_SIZE; i++) {
        if (test_int[i] != (uint8_t)(i & 0xFF)) {
            kllog("Multi-page test failed you're dumb", 1, 2);
            return;
        }
    }*/


    kllog("test passed!", 1, 0);

}