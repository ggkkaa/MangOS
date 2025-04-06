#include "page.h"
#include "utils.h"
#include "bootutils.h"
#include "memory/linked_list.h"
#include "limine/limine.h"
#include "panic.h"
#include "print.h"
#include "kernel.h"
#include <stddef.h>
#include <stdint.h>

extern struct limine_hhdm_request limine_hhdm_request;

//extern load_page_dir(uint32_t* page_directory);
//extern enable_paging();

void bootmap_physical_memory() {
        BootMemRegion region;
        for(size_t i = 0; i < boot_get_memregion_count(); ++i){
                boot_get_memregion_at(&region, i);
                
                pageflags_t flags = KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE | KERNEL_PFLAG_EXEC_DISABLE;
                
                switch (region.kind)
                {
                //case LIMINE_MEMMAP_FRAMEBUFFER:
                //        flags |= KERNEL_PFLAG_WRITE_COMBINE;
                case LIMINE_MEMMAP_USABLE:
                //case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
                //case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                //case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
                        paddr_t phys = page_align_down(region.address);
                        uintptr_t virt = page_align_down(region.address + kernel.hhdm);
                        size_t pages = region.size/PAGE_SIZE;
                        if (!page_mmap(kernel.pml4, phys, virt, pages, flags)) kpanic("Failed to initially map memory.");
                        break;
                default:
                        break;
                }
        }
}

bool page_mmap(page_t pml4_address, uintptr_t physical_addr, uintptr_t virtual_addr, size_t page_count, pageflags_t flags) {
        kllog("Mapping pages from %p to %p", 1, 0, virtual_addr, virtual_addr + page_count*PAGE_SIZE);
        virtual_addr &= ~PAGE_MASK;

        uint16_t pml1 = (virtual_addr >> (12   )) & 0x1ff;
        uint16_t pml2 = (virtual_addr >> (12+9 )) & 0x1ff;
        uint16_t pml3 = (virtual_addr >> (12+18)) & 0x1ff;
        uint16_t pml4 = (virtual_addr >> (12+27)) & 0x1ff;

        for (; pml4 < PAGE_ENTRIES; pml4++)
        {
                page_t pml3_address = NULL;
                if(pml4_address[pml4] == 0) {
                        pml4_address[pml4] = alloc_phys_pages(1);
                        if(!pml4_address[pml4]) return false;
                        pml3_address = (page_t)(pml4_address[pml4] + kernel.hhdm);
                        memset(pml3_address, 0, PAGE_SIZE);
                        pml4_address[pml4] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT | (flags & KERNEL_PFLAG_USER);
                } else {
                        pml3_address = (page_t)page_align_down(pml4_address[pml4] + kernel.hhdm);
                }

                for (; pml3 < PAGE_ENTRIES; pml3++)
                {
                        page_t pml2_address = NULL;
                        if(pml3_address[pml3] == 0) {
                                pml3_address[pml3] = alloc_phys_pages(1);
                                if(!pml3_address[pml3]) return false;
                                pml2_address = (page_t)(pml3_address[pml3] + kernel.hhdm);
                                memset(pml2_address, 0, PAGE_SIZE);
                                pml3_address[pml3] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT | (flags & KERNEL_PFLAG_USER);
                        } else {
                                pml2_address = (page_t)page_align_down(pml3_address[pml3] + kernel.hhdm);
                        }
        
                        for (; pml2 < PAGE_ENTRIES; pml2++)
                        {
                                page_t pml1_address = NULL;
                                if(pml2_address[pml2] == 0) {
                                        pml2_address[pml2] = alloc_phys_pages(1);
                                        if(!pml2_address[pml2]) return false;
                                        pml1_address = (page_t)(pml2_address[pml2] + kernel.hhdm);
                                        memset(pml1_address, 0, PAGE_SIZE);
                                        pml2_address[pml2] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT | (flags & KERNEL_PFLAG_USER);
                                } else {
                                        pml1_address = (page_t)page_align_down(pml2_address[pml2] + kernel.hhdm);
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
        
        kernel.pml4 = (page_t)(alloc_phys_pages(1) + kernel.hhdm);

        if(!kernel.pml4) {
                kpanic("Could not allocate pml4 table.");
        }

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