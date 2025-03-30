#include "../x86_64/include/gdt.h"
#include "../x86_64/include/idt.h"
#include "./memory/linked_list.h"
#include "page.h"
#include "limine/limine.h"

#define REGION_USER_STACK_ADDR  0x0000700000000000LL
#define REGION_USER_STACK_PAGES 6
#define REGION_HHDM_ADDR  0xFFFF800000000000LL
#define REGION_HHDM_PAGES 8388608
#define REGION_IO_SPACE_ADDR  0xFFFF800800000000LL
#define REGION_IO_SPACE_PAGES 524288
#define REGION_KERNEL_STACK_ADDR  0xFFFFFFFFFFFF5FFFLL
#define REGION_KERNEL_STACK_PAGES 10
#define KERNEL_MEMORY_MASK      REGION_HHDM_ADDR

extern char kernel_start;
extern char kernel_end;

typedef uint64_t paddr_t;

typedef struct 
{
    struct mem_list memory_list;
    struct GDTR gdtr;
    struct IDTR idtr;
    uint64_t available_pages;
    char* kernel_start;
    char* kernel_end;
    page_t pml4;
    struct limine_memmap_request memmap_request;
} Kernel;
extern Kernel kernel;
