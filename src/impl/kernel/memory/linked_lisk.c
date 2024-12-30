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

void init_list(struct multiboot_tag* tag) {

    kernel.available_pages = 0;

    struct mem_list *p_list;
    list_init(&p_list);
    
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
        if (type == MULTIBOOT_MEMORY_AVAILABLE) {
            kllog("Type: Available", 1, 0);

            struct list_node* current_node = (struct list_node*)base_address;

            kllog("Check 1", 1, 0);

            list_init(&current_node->list);

            kllog("Check 2", 1, 0);

            current_node->pages = length / 4096;

            kllog("Check 3", 1, 0);

            list_append(&current_node->list, &p_list->list);

            kllog("Check 4", 1, 0);
            kllog("The memory setup node is at: %p", 1, 0, current_node);
            kllog("The memory setup node is %d pages big", 1, 0, current_node->pages);

            available_memory += length;
            kernel.available_pages = available_memory / 4096;
            
            node_amount++;
        } else {    
            kllog("Type: Reserved/Other", 1, 0);
        }

        kllog("Available memory size: %d", 1, 0, available_memory);
        kllog("Available pages: %d", 1, 0, kernel.available_pages);
        mmap_entry = (struct multiboot_mmap_entry*)(((uint8_t*) mmap_entry) + tagmmap->entry_size);
    }

    kernel.memory_list = p_list;
}

