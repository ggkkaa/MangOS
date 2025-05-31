#include "x86_64/tss.h"
#include "kernel.h"
#include "x86_64/gdt.h"
#include "print.h"

extern uint8_t stack_top[];

void init_TSS() {
        kinfo("Initializing TSS");
        memset(&kernel.tss, 0, sizeof(TSS));
        kernel.tss.rsp0 = (uint64_t)stack_top;
        kernel.tss.iomap_base = sizeof(TSS);

        set_tss_descriptor((uint64_t)&kernel.tss, 5);

        asm volatile ("ltr %%ax" : : "a"(5 * 8));
}

void set_tss_descriptor(uint64_t base, int index) {
        extern uint64_t GDT[];

        uint64_t limit = sizeof(TSS) - 1;
        uint64_t low =
                (limit & 0xFFFFULL) |                           // bits 0-15: limit low
                ((base & 0xFFFFFF) << 16) |                     // bits 16-39: base low/mid
                (0x89ULL << 40) |                               // bits 40-47: type=0x9 (TSS), present
                ((limit >> 16) & 0xF) << 48 |                   // bits 48-51: limit high
                ((base >> 24) & 0xFFULL) << 56;                 // bits 56-63: base high (low half)

        uint64_t high = (base >> 32);                           // upper 32 bits of 64 bit base

        GDT[index] = low;
        GDT[index + 1] = high;

}