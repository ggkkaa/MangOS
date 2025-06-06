#include "gdt.h"
#include "print.h"
#include "../../kernel/kernel.h"

uint64_t GDT[6];

uint64_t create_gdt_entry(uint64_t base, uint64_t limit, uint64_t access, uint64_t flags) {
    uint64_t base1  = base & 0xFFFF;
    uint64_t base2  = (base >> 16) & 0xFF;
    uint64_t base3  = (base >> 24) & 0xFF;
    uint64_t limit1 = limit & 0xFFFF;
    uint64_t limit2 = limit >> 16;
    uint64_t entry  = 0;
    entry |= limit1;
    entry |= limit2 << 48;
    entry |= base1  << 16;
    entry |= base2  << 32;
    entry |= base3  << 56;
    entry |= access << 40;
    entry |= flags  << 52;
    return entry;
}

void create_system_segment_descriptor(uint64_t *GDT, uint8_t idx, uint64_t base, uint64_t limit, uint64_t access, uint64_t flags) {
    uint64_t limit1 = limit & 0xFFFF;
    uint64_t  limit2 = (limit >> 16) & 0b1111;
    uint64_t base1  = base & 0xFFFF;
    uint64_t  base2  = (base >> 16) & 0xFF;
    uint64_t  base3  = (base >> 24) & 0xFF;
    uint64_t base4  = (base >> 32) & 0xFFFFFFFF;
    GDT[idx] = 0;
    GDT[idx] |= limit1;
    GDT[idx] |= base1 << 16;
    GDT[idx] |= base2 << 32;
    GDT[idx] |= access << 40;
    GDT[idx] |= (limit2 & 0xF) << 48;
    GDT[idx] |= (flags & 0xF) << 52;
    GDT[idx] |= base3 << 56;
    GDT[idx + 1] = base4;
}

__attribute__((noinline))
void init_GDT() {
    kllog("Initiating GDT... ", 1, 0);
    GDT[0] = create_gdt_entry(0, 0, 0, 0); // null
    GDT[1] = create_gdt_entry(0, 0, 0x9A, 0x2); // kernel code
    GDT[2] = create_gdt_entry(0, 0, 0x92, 0); // kernel data
    GDT[3] = create_gdt_entry(0, 0, 0xFA, 0x2); // user code
    GDT[4] = create_gdt_entry(0, 0, 0xF2, 0); // user data
    kllog("GDT entries created", 1, 0);
    kernel.gdtr.size   = sizeof(GDT) - 1;
    kernel.gdtr.offset = (uint64_t) GDT;
    asm("lgdt (%0)" : : "r" (&kernel.gdtr));  
    asm volatile("push $0x08; \
              lea .reload_CS(%%rip), %%rax; \
              push %%rax; \
              retfq; \
              .reload_CS: \
              mov $0x10, %%ax; \
              mov %%ax, %%ds; \
              mov %%ax, %%es; \
              mov %%ax, %%fs; \
              mov %%ax, %%gs; \
              mov %%ax, %%ss" : : : "eax", "rax");
    kllog("GDT has fully initialized", 1, 0);
}