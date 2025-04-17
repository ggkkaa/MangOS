#include <stdbool.h>
#include "utils.h"

typedef uint64_t *page_t;

// This piece of code has been copied from https://github.com/Dcraftbg/MinOS because I'm way too lazy to make this by myself
#define PAGE_MASK 0xFFFF000000000000

typedef uint64_t pageflags_t;

#define KERNEL_PTYPE_SHIFT 9
#define KERNEL_PFLAG_PRESENT       0b1
#define KERNEL_PFLAG_WRITE         0b10
#define KERNEL_PFLAG_USER          0b100
#define KERNEL_PFLAG_WRITE_THROUGH 0b1000
#define KERNEL_PFLAG_WRITE_COMBINE 0b10000000
#define KERNEL_PFLAG_CACHE_DISABLE 0b10000
#define KERNEL_PFLAG_EXEC_DISABLE  0// 0b1000000000000000000000000000000000000000000000000000000000000000
// NOTE: Dirty
#define KERNEL_PFLAG_ACCESSED      0b100000

#define KENREL_PTYPE_MASK 0b111000000000

#define KERNEL_PTYPE_KERNEL  (0b0   << KERNEL_PTYPE_SHIFT)
#define KERNEL_PTYPE_USER    (0b111 << KERNEL_PTYPE_SHIFT)

#define KERNEL_PFLAGS_MASK 0b1000000000000000000000000000000000000000000000000000111111111111LL
// Technically incorrect but idrk
#define KERNEL_PADDR_MASK  0b0111111111111111111111111111111111111111111111111111000000000000LL 
// End code copied from MinOS.

#define PAGE_ENTRIES 512
#define PHYS_MEM_RESERVE_SIZE (4LLU * 1024LLU * 1024LLU * 1024LLU) 

void init_paging();
bool page_mmap(page_t pml4_address, uintptr_t physical_addr, uintptr_t virtual_addr, size_t page_count, pageflags_t flags);
bool alloc_pages(page_t pml4_address, uintptr_t virtual_addr, size_t page_count, pageflags_t flags);