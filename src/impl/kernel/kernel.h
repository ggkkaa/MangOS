#include "gdt.h"
#include "idt.h"
#include "./memory/linked_list.h"

extern char* kernel_start;
extern char* kernel_end;

typedef struct 
{
    struct mem_list* memory_list;
    struct GDTR gdtr;
    struct IDTR idtr;
    uint64_t available_pages;

} Kernel;
extern Kernel kernel;
