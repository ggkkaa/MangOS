#include "kernel.h"
#include "utils.h"
#include "limine/limine.h"
#include "kassert.h"

void enable_interrupts() {
    asm volatile("sti");
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

int strcmp(const char *a, const char *b) {
        size_t i = 0;
        while (a[i] && b[i]) {
                if (a[i] != b[i]) return a[i] - b[i];
                i++;
        }
        return a[i] - b[i];
}

char* strncpy(char* dest, const char* src, size_t n) {
        size_t i;

        for (i = 0; i < n && src[i] != '\0'; i++) {
                dest[i] = src[i];
        }

        for (; i < n; i++) {
                dest[i] = '\0';
        }

        return dest;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;

    for (; *haystack; haystack++) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && *h == *n) {
            h++;
            n++;
        }
        if (!*n) return (char*)haystack;
    }

    return NULL;
}
