#pragma once
#include <stdint.h>

struct IDT_entry {
   uint16_t offset_1;
   uint16_t selector;
   uint8_t  ist;
   uint8_t  flags;
   uint16_t offset_2;
   uint32_t offset_3;
   uint32_t reserved;
} __attribute__((packed));

struct IDTR {
    uint16_t size;
    uint64_t offset;
} __attribute__((packed));

void create_IDT_entry(uint32_t vector, void* isr, uint8_t flags, struct IDT_entry *IDT);
void init_IDT();