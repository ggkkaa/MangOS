#pragma once
#include <stdint.h>
#include <stddef.h>
#include "list.h"
#include "multiboot/multiboot2.h"
#include "utils.h"
#include "strconvert.h"
#define PAGE_SIZE 4096

struct list_node {
    struct list list;
    size_t pages;
} __attribute__ ((aligned (PAGE_SIZE)));


struct mem_list {
    struct list list;
};

void* alloc_phys_pages(size_t pages_count);
void free_phys_pages(void* address, size_t pages_count);
void init_list(struct multiboot_tag* tag);
void allocator_test();


/*
    pointer to sth or sth idk??
*/