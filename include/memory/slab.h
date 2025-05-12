#pragma once

#include <stdint.h>
#include "list.h"

#define MAX_CACHE_NAME 20

typedef struct Cache
{
    struct list list;

    struct list full, partial, empty;

    char cache_name[MAX_CACHE_NAME];

    size_t used;

    size_t num_objs;
    size_t obj_size;

    size_t num_active_slabs;
    size_t total_slabs;
    size_t objects_per_slab;
} Cache;

typedef struct Slab
{
    struct list list;
    void *mem;
    size_t used;

    size_t free;
} Slab;

#define slab_bufctl(slab) ((uint32_t *)(slab)+1)

void init_cache(Cache* cache, size_t obj_size);
Cache* cache_create(size_t obj_size, const char* name);
void* cache_alloc(Cache* cache);
void cache_free(Cache* cache, void* ptr);
void init_slab_allocator(void);