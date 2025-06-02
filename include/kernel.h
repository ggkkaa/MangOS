#include "x86_64/gdt.h"
#include "x86_64/idt.h"
#include "memory/linked_list.h"
#include "memory/page.h"
#include "memory/slab.h"
#include "limine/limine.h"
#include "drivers/framebuffer.h"
#include "x86_64/tss.h"
#include "drivers/fs/vfs.h"


#define REGION_USER_STACK_ADDR  0x0000700000000000LL
#define REGION_USER_STACK_PAGES 6
#define REGION_HHDM_ADDR  0xFFFF800000000000LL
#define REGION_HHDM_PAGES 8388608
#define REGION_IO_SPACE_ADDR  0xFFFF800800000000LL
#define REGION_IO_SPACE_PAGES 524288
#define REGION_KERNEL_STACK_ADDR  0xFFFFFFFFFFFF5FFFLL
#define REGION_KERNEL_STACK_PAGES 10
#define KERNEL_MEMORY_MASK      REGION_HHDM_ADDR

typedef struct 
{
    uintptr_t hhdm;
    struct mem_list memory_list;
    struct GDTR gdtr;
    struct IDTR idtr;
        __attribute__((aligned(16)))
        TSS tss;
    Framebuffer framebuffer;
    Cursor cursor;
    uint64_t available_pages;
    char* kernel_start;
    char* kernel_end;
    page_t pml4;
    paddr_t phys_addr;
    void* virt_addr;
    struct list caches;
    Cache* slab_cache;
    Cache* inode_cache;
    Superblock root_block;
} Kernel;
extern Kernel kernel;
