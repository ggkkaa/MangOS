#include <stdint.h>

struct InterruptDescriptor64 {
   uint16_t offset_1;
   uint16_t selector;
   uint8_t  ist;
   uint8_t  type_attributes;
   uint16_t offset_2;
   uint32_t offset_3;
   uint32_t reserved;
} __attribute__((packed));