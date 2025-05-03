#pragma once
#define true 1

#include <stddef.h>
#include <stdint.h>

#define page_align_up(p)   (((p) + (PAGE_SIZE-1)) / PAGE_SIZE * PAGE_SIZE)
#define page_align_down(p) (((p)                ) / PAGE_SIZE * PAGE_SIZE)
#define round_to_page(size) (((size) + (PAGE_SIZE) - 1) / (PAGE_SIZE))


typedef struct
{
        uintptr_t virtual;
        uintptr_t physical;
} memory_pair;


void enable_interrupts();
size_t str_len(const char* str);
void *memcpy(void *dest, const void *src, size_t size);
void *memset(void *pointer, int val, size_t size);
void *memmove(void *dest, const void *src, size_t size);
int memcmp(const void *pointer1, const void *pointer2, size_t size);
void kernel_mempair(memory_pair* mempair);    

#define disable_interrupts() asm volatile("cli")
#define wait_for_interrupt() asm volatile("hlt")

#define halt() \
    disable_interrupts(); \
    for (;;) { \
        wait_for_interrupt(); \
    }
