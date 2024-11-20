#include "idt.h"
#include "../../kernel/kernel.h"
#include "print.h"

void create_IDT_entry(uint32_t vector, void* isr, uint8_t flags, struct IDT_entry *IDT) {
    IDT[vector].offset_1 = (uint64_t)isr;
    IDT[vector].offset_2 = ((uint64_t)isr) >> 16;
    IDT[vector].offset_3 = ((uint64_t)isr) >> 32;
    IDT[vector].flags = flags;
    IDT[vector].selector = 0x08;
}

extern division_exception();


struct IDT_entry *IDT[256];

void init_IDT() {
    printf("Initializing the IDT!\n");
    create_IDT_entry(0, &division_exception, 0x8E, IDT);
    printf("IDT entries created!\n");
    kernel.idtr.size = (sizeof(struct IDT_entry) * 256) - 1;
    kernel.idtr.offset = (uint64_t) IDT;
    asm("lidt %0" : : "m" (kernel.idtr));
    printf("IDT has been created.\n");
}