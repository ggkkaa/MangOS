#pragma once
#include <stdint.h>
#include <stddef.h>
#include "list.h"
#include "../multiboot2.h"

struct list_node {
    struct list list;
    size_t pages;
};


struct mem_list {
    struct list list;
};

void* kmalloc(size_t pages_count);
void kfree(void* address, size_t pages_count);
void init_list(struct multiboot_tag* tag);


/*
    pointer to sth or sth idk??
*/