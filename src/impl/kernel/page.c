#include "page.h"
#include "utils.h"
#include "memory/linked_list.h"
#include "limine/limine.h"
#include "panic.h"
#include "print.h"
#include "kernel.h"
#include <stddef.h>
#include <stdint.h>


//extern load_page_dir(uint32_t* page_directory);
//extern enable_paging();

void bootmap_physical_memory() {
        kllog("entries array is at: %p", 1, 0, kernel.memmap_request.response->entries);
        for (size_t i = 0; i < kernel.memmap_request.response->entry_count; ++i) {
            kllog("entry[%d] = %p", 1, 0, i, kernel.memmap_request.response->entries[i]);
        }

        for(size_t i = 0; i < kernel.memmap_request.response->entry_count; ++i){
                struct limine_memmap_entry* entry = kernel.memmap_request.response->entries[i];
                
                pageflags_t flags = KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE | KERNEL_PFLAG_EXEC_DISABLE;
                size_t pages = entry->length/PAGE_SIZE;
                
                if(entry->type != LIMINE_MEMMAP_FRAMEBUFFER && entry->type != LIMINE_MEMMAP_EXECUTABLE_AND_MODULES) {
                        if(entry->base >= PHYS_MEM_RESERVE_SIZE) {
                                kllog("Out Of Bounds", 1, 0);
                                continue;
                        }
                        if(entry->base + (pages*PAGE_SIZE) > PHYS_MEM_RESERVE_SIZE) {
                                pages = (PHYS_MEM_RESERVE_SIZE - entry->base)/PAGE_SIZE;
                        }
                }
                
                switch (entry->type)
                {
                //case LIMINE_MEMMAP_FRAMEBUFFER:
                //        flags |= KERNEL_PFLAG_WRITE_COMBINE;
                case LIMINE_MEMMAP_USABLE:
                //case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
                //case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                //case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
                kllog("Mapping %d pages from %p to %p.", 1, 0, pages, page_align_down(entry->base | KERNEL_MEMORY_MASK), page_align_down(entry->base | KERNEL_MEMORY_MASK) + pages*PAGE_SIZE);
                        if(!page_mmap(kernel.pml4, page_align_down(entry->base), page_align_down(entry->base | KERNEL_MEMORY_MASK), pages, flags)) {
                                kpanic("Could not map memory.");
                        }
                        break;
                default:
                        kllog("Skipping %d pages.", 1, 0, pages);
                }

                kllog("The next 3 memory entries are %p, %p, and %p", 1, 0, kernel.memmap_request.response->entries[i],kernel.memmap_request.response->entries[i+1],kernel.memmap_request.response->entries[i+2]);
        }
}

bool page_mmap(page_t pml4_address, uintptr_t physical_addr, uintptr_t virtual_addr, size_t page_count, pageflags_t flags) {
        kllog("Mapping pages from %p to %p", 1, 0, virtual_addr, virtual_addr + page_count*PAGE_SIZE);
        virtual_addr &= ~PAGE_MASK;
        physical_addr &= ~KERNEL_MEMORY_MASK;

        uint16_t pml1 = (virtual_addr >> (12   )) & 0x1ff;
        uint16_t pml2 = (virtual_addr >> (12+9 )) & 0x1ff;
        uint16_t pml3 = (virtual_addr >> (12+18)) & 0x1ff;
        uint16_t pml4 = (virtual_addr >> (12+27)) & 0x1ff;

        for (; pml4 < PAGE_ENTRIES; pml4++)
        {
                page_t pml3_address = NULL;
                if(pml4_address[pml4] == 0) {
                        pml4_address[pml4] = (uintptr_t)((long long unsigned int)alloc_phys_pages(1) | KERNEL_MEMORY_MASK);
                        if(!pml4_address[pml4]) return false;
                        pml3_address = (page_t)(pml4_address[pml4] | KERNEL_MEMORY_MASK);
                        memset(pml3_address, 0, PAGE_SIZE);
                        pml4_address[pml4] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT | (flags & KERNEL_PFLAG_USER);
                } else {
                        pml3_address = (page_t)page_align_down(pml4_address[pml4] | KERNEL_MEMORY_MASK);
                }

                for (; pml3 < PAGE_ENTRIES; pml3++)
                {
                        page_t pml2_address = NULL;
                        if(pml3_address[pml3] == 0) {
                                pml3_address[pml3] = (uintptr_t)((long long unsigned int)alloc_phys_pages(1) | KERNEL_MEMORY_MASK);
                                if(!pml3_address[pml3]) return false;
                                pml2_address = (page_t)(pml3_address[pml3] | KERNEL_MEMORY_MASK);
                                memset(pml2_address, 0, PAGE_SIZE);
                                pml3_address[pml3] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT | (flags & KERNEL_PFLAG_USER);
                        } else {
                                pml2_address = (page_t)page_align_down(pml3_address[pml3] | KERNEL_MEMORY_MASK);
                        }
        
                        for (; pml2 < PAGE_ENTRIES; pml2++)
                        {
                                page_t pml1_address = NULL;
                                if(pml2_address[pml2] == 0) {
                                        pml2_address[pml2] = (uintptr_t)((long long unsigned int)alloc_phys_pages(1) | KERNEL_MEMORY_MASK);
                                        if(!pml2_address[pml2]) return false;
                                        pml1_address = (page_t)(pml2_address[pml2] | KERNEL_MEMORY_MASK);
                                        memset(pml1_address, 0, PAGE_SIZE);
                                        pml2_address[pml2] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT | (flags & KERNEL_PFLAG_USER);
                                } else {
                                        pml1_address = (page_t)page_align_down(pml2_address[pml2] | KERNEL_MEMORY_MASK);
                                }
                
                                for (; pml1 < PAGE_ENTRIES; pml1++)
                                {
                                        pml1_address[pml1] = physical_addr | flags;
                                        page_count--;
                                        physical_addr += PAGE_SIZE;
                                        if(page_count == 0) return true;
                                }
                                pml1 = 0;
                        }
                        pml2 = 0;
                }
                pml3 = 0;
        }
        return page_count == 0;
        
}

extern uint8_t section_text_begin[];
extern uint8_t section_text_end[];


void init_paging() {
        kllog("Initializing page lists", 1, 0);
        memory_pair addr_resp = {0};
        
        kernel_mempair(&addr_resp);
        
        paddr_t pml4_physical = (paddr_t)alloc_phys_pages(1);

        if(!pml4_physical) {
                kpanic("Could not allocate pml4 table.");
        }

        kernel.pml4 = (page_t)(pml4_physical | KERNEL_MEMORY_MASK);
        memset(kernel.pml4, 0, PAGE_SIZE);
        bootmap_physical_memory();

        halt();

        /*uintptr_t physical, virtual;
        size_t length;

        physical = page_align_down(addr_resp.physical + (((uintptr_t)section_text_begin) - addr_resp.virtual));
        virtual = page_align_down((uintptr_t)section_text_begin);
        length  = (page_align_up((uintptr_t)section_text_end) - page_align_down((uintptr_t)section_text_begin))/PAGE_SIZE;
        kllog("mapping .info, %d pages", 1, 0, length);

        if(!page_mmap())

        kllog("Loading page directory", 1, 0);
        //load_page_dir(&page_directory);
        kllog("Enabling Paging", 1, 0);
        //enable_paging();

        kllog("Paging initialization finished!", 1, 0);
        */
}