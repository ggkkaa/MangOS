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

// Global variable that stores the amount of nodes
uint64_t node_amount = 0;

/*
    Initializes the lined list for physical allocation.

    First, it initializes the list and it reads the mmap tags.

    Then, it prints them out and assigns a list node at the
    beggining of each

*/

void init_list() {
        kpanic("TODO: Implement init_list");
        
    /*kernel.available_pages = 0;

    list_init(&kernel.memory_list.list);

    uint64_t available_memory = 0;

    struct multiboot_tag_mmap* tagmmap = (struct multiboot_tag_mmap*) tag;
    
    struct multiboot_mmap_entry* mmap_entry = tagmmap->entries;
    
    // Iterate over the mmap entry to print debugging information and set up a basic linked list
    while (((uint8_t*) mmap_entry) < ((uint8_t*) tagmmap) + tagmmap->size) {
        uint64_t base_address = mmap_entry->addr;
        uint64_t length = mmap_entry->len;
        uint32_t type = mmap_entry->type;

        kllog("Memory address at %p found", 1, 0, base_address);
        kllog("Length of memory address is %d", 1, 0, length);

        //kllog("Marking areas %p to %p as unavailable...", 1, 0, kernel.kernel_start, kernel.kernel_end);


        // If the address is available then mark it as empty, otherwise mark it as used.
        if (type == MULTIBOOT_MEMORY_AVAILABLE) { //If the memory is available,
            kllog("Type: Available", 1, 0); // Say that it's available

            if ((base_address >= (uintptr_t)kernel.kernel_start) && (base_address <= (uintptr_t)kernel.kernel_end)) {

                uint64_t k_before = base_address - (uintptr_t)kernel.kernel_start;
                uint64_t k_after = (uintptr_t)kernel.kernel_end - base_address;

                kllog("There are %d bytes before the kernel starts.", 1, 0, k_before);
                kllog("There are %d bytes after the kernel ends.", 1, 0, k_after);

                if (k_before > 0)
                {
                        // set up a node for the free spot in memory,
                        struct list_node* current_node = (struct list_node*)base_address;

                        list_init(&current_node->list);

                        current_node->pages = k_before / PAGE_SIZE;

                        list_append(&current_node->list, &kernel.memory_list.list);

                        // print out the node that was set up
                        kllog("The memory setup node is at: %p", 1, 0, current_node);
                        kllog("The memory setup node is %d pages big", 1, 0, current_node->pages);

                        // And modify kernel values.
                        available_memory += length;
                        kernel.available_pages = available_memory / PAGE_SIZE;
                        
                        node_amount++;
                }

                if (k_after > 0)
                {
                        // set up a node for the free spot in memory,
                        struct list_node* current_node = (struct list_node*)kernel.kernel_end;

                        list_init(&current_node->list);

                        current_node->pages = k_after / PAGE_SIZE;

                        list_append(&current_node->list, &kernel.memory_list.list);

                        // print out the node that was set up
                        kllog("The memory setup node is at: %p", 1, 0, current_node);
                        kllog("The memory setup node is %d pages big", 1, 0, current_node->pages);

                        // And modify kernel values.
                        available_memory += length;
                        kernel.available_pages = available_memory / PAGE_SIZE;
                        
                        node_amount++;
                }
                
            } else {

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
            }
        } else {    // Else, say that the memory is not available
            kllog("Type: Reserved/Other", 1, 0);
        }

        // Print out some information
        kllog("Available memory size: %d", 1, 0, available_memory);
        kllog("Available pages: %d", 1, 0, kernel.available_pages);
        mmap_entry = (struct multiboot_mmap_entry*)(((uint8_t*) mmap_entry) + tagmmap->entry_size);
    }
        kllog("Node Amount: %d", 1, 0, node_amount);

        for(struct list* list = kernel.memory_list.list.next; list != &kernel.memory_list.list; list = list->next) {
                struct list_node* node = (struct list_node*)list;
                kllog("List node at %p found.", 1, 0, node);
        }*/
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
    if(pages_count == 1) {return alloc_phys_page;}
    for(struct list* list = kernel.memory_list.list.next; list != &kernel.memory_list.list; list = list->next) {
        struct list_node* node = (struct list_node*)list;
        void* result = node;
        if(node->pages > pages_count) {
            struct list_node* new_node = (struct list_node*)((char*)node + PAGE_SIZE*pages_count);
            new_node->pages = node->pages - pages_count;
            list_init(&new_node->list);
            list_append(&new_node->list, &node->list);
            list_remove(&node->list);
            return result;
        } else if(node->pages) {
            list_remove(list);
            return result;
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
        
        kllog("Write %d. %d Written.", 1, 0, i, test_int[i]);
    }

    kllog("Pattern filled.", 1, 0);

    for (size_t i = 0; i < PAGE_SIZE; i++) {
        //kllog("At offset %d,", 1, 0, i);
        if (test_int[i] != (uint16_t)(i & 0xFF)) {
            success = false;
            //kllog("Memory test failed at byte %p: Expected %d, got %d", 1, 4, i + test_int, (uint16_t)(i & 0xFF), test_int[i]);
            break;
        }
            // Buffer sizes for borders
            //int addr_len = 23;
            //int value_len = str_len(int_to_str(test_int[i]));
            //int content_length = addr_len + value_len;

            
            //for (int j = 0; j < content_length; j++) k_serial_printf("_");

            
            //k_serial_printf("\n|Byte %p|%d|\n", &test_int[i], test_int[i]);

            //for (int j = 0; j < content_length; j++) k_serial_printf("-");
            //k_serial_printf("\n");
    }
    if(success == false) {
        kllog("Error! Memory allocation test failed!!!", 1, 2);
        kpanic("Single Page Allocation Failed");
    } else {
        kllog("Single page physical allocation test finished.", 1, 0);
    }


    //kllog("Multi page physical allocation test starting...", 1, 0);

    //test_int = (uint16_t*)alloc_phys_pages(TEST_ALLOC_SIZE);

    /*for (size_t i = 0; i < TEST_ALLOC_SIZE; i++) {
        if ((uintptr_t)test_int[i] % PAGE_SIZE != 0) {
            kllog("Page is not aligned!", 1, 1);
            return;
        }
        test_int[i] = (uint16_t)(i & 0xFF);
        
    }


    for (size_t i = 0; i < TEST_ALLOC_SIZE; i++) {
        if (test_int[i] != (uint16_t)(i & 0xFF)) {
            kllog("Multi-page test failed you're dumb", 1, 2);
            return;
        }
    }*/


    kllog("test passed!", 1, 0);

}