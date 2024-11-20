#include "gdt.h"
#include "idt.h"

typedef struct 
{
    struct GDTR gdtr;
    struct IDTR idtr;
} Kernel;

extern Kernel kernel;
