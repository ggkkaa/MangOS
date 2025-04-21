#include "memory/page.h"
#include "bootutils.h"
#include "kernel.h"
#include "limine/limine.h"
#include "memory/linked_list.h"
#include "panic.h"
#include "print.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

extern struct limine_hhdm_request limine_hhdm_request;

extern uint64_t KERNEL_START[];
extern uint64_t WRITE_ALLOWED_START[];
extern uint64_t KERNEL_END[];

// extern load_page_dir(uint32_t* page_directory);
// extern enable_paging();

void map_kernel() {
  uint64_t length =
      page_align_up((uintptr_t)WRITE_ALLOWED_START - (uintptr_t)KERNEL_START) / PAGE_SIZE;
  uint64_t phys = kernel.phys_addr + ((uintptr_t)KERNEL_START - (uintptr_t)kernel.virt_addr);

  kinfo("Mapping read-only part of the kernel. Physical address %p to virtual "
        "address %p, %d pages.",
        phys, page_align_down((uintptr_t)KERNEL_START), length);
  page_mmap(kernel.pml4, phys, page_align_down((uintptr_t)KERNEL_START), length, KERNEL_PFLAG_PRESENT);

  length =
      page_align_up((uintptr_t)KERNEL_END - (uintptr_t)WRITE_ALLOWED_START) /
      PAGE_SIZE;
  phys = kernel.phys_addr +
         ((uintptr_t)WRITE_ALLOWED_START - (uintptr_t)kernel.virt_addr);

<<<<<<< HEAD
        length = page_align_up((uintptr_t)KERNEL_END - (uintptr_t)WRITE_ALLOWED_START) / PAGE_SIZE;
        phys = kernel.phys_addr + ((uintptr_t)WRITE_ALLOWED_START - (uintptr_t)kernel.virt_addr);

        kinfo("Mapping writeable part of the kernel. Physical address %p to virtual address %p, %d pages.", phys, page_align_down((uintptr_t)WRITE_ALLOWED_START), length);
        page_mmap(kernel.pml4, phys, page_align_down((uintptr_t)WRITE_ALLOWED_START), length, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);
<<<<<<< HEAD

        kinfo("Mapping %p to %p for the kernel stack.", KERNEL_STACK_ADDR, KERNEL_STACK_PTR);
        alloc_pages(kernel.pml4, KERNEL_STACK_ADDR, KERNEL_STACK_PAGES, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);
=======
>>>>>>> 4782054 (stuff)
=======
  kinfo("Mapping writeable part of the kernel. Physical address %p to virtual "
        "address %p, %d pages.",
        phys, page_align_down((uintptr_t)WRITE_ALLOWED_START), length);
  page_mmap(kernel.pml4, phys, page_align_down((uintptr_t)WRITE_ALLOWED_START),
            length, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);
>>>>>>> af3de54 (Fix page fault)
}

void map_all() {
  BootMemRegion region;
  for (size_t i = 0; i < boot_get_memregion_count(); ++i) {
    boot_get_memregion_at(&region, i);

    pageflags_t flags =
        KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE | KERNEL_PFLAG_EXEC_DISABLE;

    switch (region.kind) {
    case LIMINE_MEMMAP_FRAMEBUFFER:
    case LIMINE_MEMMAP_USABLE:
    case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
    case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
      paddr_t phys = page_align_down(region.address);
      uintptr_t virt = page_align_down(region.address + kernel.hhdm);
      size_t pages = region.size / PAGE_SIZE;
      if (!page_mmap(kernel.pml4, phys, virt, pages, flags))
        kpanic("Failed to initially map memory.");
      break;
    default:
      break;
    }
  }
  kinfo("Finished mapping stuff, time to map the kernel.");
  map_kernel();
}

<<<<<<< HEAD
<<<<<<< HEAD
bool page_mmap(page_t pml4_address, uintptr_t physical_addr, uintptr_t virtual_addr, size_t page_count, pageflags_t flags) {
=======
bool page_mmap(page_t pml4_address, paddr_t physical_addr, uintptr_t virtual_addr, size_t page_count, pageflags_t flags) {
>>>>>>> 4782054 (stuff)
        kinfo("Mapping pages from %p to %p", virtual_addr, virtual_addr + page_count*PAGE_SIZE);
        virtual_addr &= ~PAGE_MASK;
=======
bool page_mmap(page_t pml4_address, paddr_t physical_addr,
               uintptr_t virtual_addr, size_t page_count, pageflags_t flags) {
  kinfo("Mapping pages from %p to %p", virtual_addr,
        virtual_addr + page_count * PAGE_SIZE);
  virtual_addr &= ~PAGE_MASK;
>>>>>>> af3de54 (Fix page fault)

  uint16_t pml1 = (virtual_addr >> (12)) & 0x1ff;
  uint16_t pml2 = (virtual_addr >> (12 + 9)) & 0x1ff;
  uint16_t pml3 = (virtual_addr >> (12 + 18)) & 0x1ff;
  uint16_t pml4 = (virtual_addr >> (12 + 27)) & 0x1ff;

  for (; pml4 < PAGE_ENTRIES; pml4++) {
    page_t pml3_address = NULL;
    if (pml4_address[pml4] == 0) {
      pml4_address[pml4] = alloc_phys_pages(1);
      if (!pml4_address[pml4])
        return false;
      pml3_address = (page_t)(pml4_address[pml4] + kernel.hhdm);
      memset(pml3_address, 0, PAGE_SIZE);
      pml4_address[pml4] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT |
                            (flags & KERNEL_PFLAG_USER);
    } else {
      pml3_address = (page_t)page_align_down(pml4_address[pml4] + kernel.hhdm);
    }

    for (; pml3 < PAGE_ENTRIES; pml3++) {
      page_t pml2_address = NULL;
      if (pml3_address[pml3] == 0) {
        pml3_address[pml3] = alloc_phys_pages(1);
        if (!pml3_address[pml3])
          return false;
        pml2_address = (page_t)(pml3_address[pml3] + kernel.hhdm);
        memset(pml2_address, 0, PAGE_SIZE);
        pml3_address[pml3] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT |
                              (flags & KERNEL_PFLAG_USER);
      } else {
        pml2_address =
            (page_t)page_align_down(pml3_address[pml3] + kernel.hhdm);
      }

      for (; pml2 < PAGE_ENTRIES; pml2++) {
        page_t pml1_address = NULL;
        if (pml2_address[pml2] == 0) {
          pml2_address[pml2] = alloc_phys_pages(1);
          if (!pml2_address[pml2])
            return false;
          pml1_address = (page_t)(pml2_address[pml2] + kernel.hhdm);
          memset(pml1_address, 0, PAGE_SIZE);
          pml2_address[pml2] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT |
                                (flags & KERNEL_PFLAG_USER);
        } else {
          pml1_address =
              (page_t)page_align_down(pml2_address[pml2] + kernel.hhdm);
        }

        for (; pml1 < PAGE_ENTRIES; pml1++) {
          pml1_address[pml1] = physical_addr | flags;
          page_count--;
          physical_addr += PAGE_SIZE;
          if (page_count == 0)
            return true;
        }
        pml1 = 0;
      }
      pml2 = 0;
    }
    pml3 = 0;
  }
  return page_count == 0;
}

bool alloc_pages(page_t pml4_address, uintptr_t virtual_addr, size_t page_count,
                 pageflags_t flags) {
  kinfo("Allocating %d page(s) at %p", page_count, virtual_addr);
  virtual_addr &= ~PAGE_MASK;

  uint16_t pml1 = (virtual_addr >> (12)) & 0x1ff;
  uint16_t pml2 = (virtual_addr >> (12 + 9)) & 0x1ff;
  uint16_t pml3 = (virtual_addr >> (12 + 18)) & 0x1ff;
  uint16_t pml4 = (virtual_addr >> (12 + 27)) & 0x1ff;

  for (; pml4 < PAGE_ENTRIES; pml4++) {
    page_t pml3_address = NULL;
    if (pml4_address[pml4] == 0) {
      pml4_address[pml4] = alloc_phys_pages(1);
      if (!pml4_address[pml4])
        return false;
      pml3_address = (page_t)(pml4_address[pml4] + kernel.hhdm);
      memset(pml3_address, 0, PAGE_SIZE);
      pml4_address[pml4] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT |
                            (flags & KERNEL_PFLAG_USER);
    } else {
      pml3_address = (page_t)page_align_down(pml4_address[pml4] + kernel.hhdm);
    }

    for (; pml3 < PAGE_ENTRIES; pml3++) {
      page_t pml2_address = NULL;
      if (pml3_address[pml3] == 0) {
        pml3_address[pml3] = alloc_phys_pages(1);
        if (!pml3_address[pml3])
          return false;
        pml2_address = (page_t)(pml3_address[pml3] + kernel.hhdm);
        memset(pml2_address, 0, PAGE_SIZE);
        pml3_address[pml3] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT |
                              (flags & KERNEL_PFLAG_USER);
      } else {
        pml2_address =
            (page_t)page_align_down(pml3_address[pml3] + kernel.hhdm);
      }

      for (; pml2 < PAGE_ENTRIES; pml2++) {
        page_t pml1_address = NULL;
        if (pml2_address[pml2] == 0) {
          pml2_address[pml2] = alloc_phys_pages(1);
          if (!pml2_address[pml2])
            return false;
          pml1_address = (page_t)(pml2_address[pml2] + kernel.hhdm);
          memset(pml1_address, 0, PAGE_SIZE);
          pml2_address[pml2] |= KERNEL_PFLAG_WRITE | KERNEL_PFLAG_PRESENT |
                                (flags & KERNEL_PFLAG_USER);
        } else {
          pml1_address =
              (page_t)page_align_down(pml2_address[pml2] + kernel.hhdm);
        }

        for (; pml1 < PAGE_ENTRIES; pml1++) {
          paddr_t physical_addr = alloc_phys_pages(1);
          memset((uint8_t *)(physical_addr + kernel.hhdm), 0, PAGE_SIZE);
          pml1_address[pml1] = physical_addr | flags;
          page_count--;
          physical_addr += PAGE_SIZE;
          if (page_count == 0)
            return true;
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
  kinfo("Initializing page lists");
  memory_pair addr_resp = {0};

  kernel_mempair(&addr_resp);

  kernel.pml4 = (page_t)(alloc_phys_pages(1) + kernel.hhdm);

  if (!kernel.pml4) {
    kpanic("Could not allocate pml4 table.");
  }

  memset(kernel.pml4, 0, PAGE_SIZE);
  map_all();
  kinfo("Set up paging!!!");
}
