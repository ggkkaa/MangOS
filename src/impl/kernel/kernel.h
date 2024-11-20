#include "gdt.h"

typedef struct 
{
    struct GDTR gdtr;
} Kernel;

extern Kernel kernel;
