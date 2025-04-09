#include "kernel.h"
#include "utils.h"
#include "limine/limine.h"
#include "kassert.h"

void disable_interrupts(){
    asm volatile("cli");
}
void enable_interrupts() {
    asm volatile("sti");
}
void wait_for_interrupt() {
    asm volatile("hlt");
}
void halt() {
    disable_interrupts();
    for (;;)
    {
        wait_for_interrupt();
    }
    
}

size_t str_len(const char* str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

void *memcpy(void *dest, const void *src, size_t size) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    for (size_t i = 0; i < size; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *pointer, int val, size_t size) {
    uint8_t *p = (uint8_t *)pointer;

    for (size_t i = 0; i < size; i++) {
        p[i] = (uint8_t)val;
    }

    return pointer;
}

void *memmove(void *dest, const void *src, size_t size) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < size; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = size; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *pointer1, const void *pointer2, size_t size) {
    const uint8_t *p1 = (const uint8_t *)pointer1;
    const uint8_t *p2 = (const uint8_t *)pointer2;

    for (size_t i = 0; i < size; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

void kernel_mempair(memory_pair* mempair) {
        mempair->virtual = (uintptr_t)kernel.virt_addr;
        mempair->physical = (uintptr_t)kernel.phys_addr;
    }
    