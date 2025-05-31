#pragma once
#include <stdint.h>

typedef struct TSS
{
        uint32_t reserved0;
        uint64_t rsp0;
        uint64_t rsp1;
        uint64_t rsp2;
        uint64_t reserved1;
        uint64_t ist[7];
        uint64_t reserved2;
        uint16_t reserved3;
        uint16_t iomap_base;
}__attribute__((packed)) TSS;

void init_TSS();
void set_tss_descriptor(uint64_t base, int index);