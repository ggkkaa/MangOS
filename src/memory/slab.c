#include "kernel.h"
#include "utils.h"
#include "print.h"
#include "memory/slab.h"
#include "memory/linked_list.h"
#include "memory/memory.h"
#include "kassert.h"
#include <stddef.h>

size_t calculate_slab_size(Cache *cache) {
    return cache->objects_per_slab * (cache->obj_size + sizeof(uint32_t)) + sizeof(Slab);
}

intptr_t cache_grow(Cache *cache) {
    void *mem = kernel_malloc(calculate_slab_size(cache));
    kinfo("Allocating slab of size: %d", calculate_slab_size(cache));

    if (!mem) {
        kerror("Memory initialization failed!!!");
        return -1;    
    };
    
    Slab *slab = mem;
    
    slab->mem = ((uint32_t*)(mem+sizeof(*slab)))+cache->objects_per_slab;

    slab->free = 0;

    cache->total_slabs += 1;

    for (size_t i = 0; i < cache->objects_per_slab; i++) {
        slab_bufctl(slab)[i] = i;
    }
        kinfo("Slab memory address: %p", slab->mem);
        kinfo("Slab address: %p", slab);
        list_append(&slab->list, &cache->empty);

        uint8_t* s = (uint8_t*)slab;
        kinfo("slab %p raw dump:", slab);
        for (int i = 0; i < 128; i += 8) {
        kinfo(" +%x: %x %x %x %x %x %x %x %x", i,
                s[i], s[i+1], s[i+2], s[i+3],
                s[i+4], s[i+5], s[i+6], s[i+7]);
        }

    return 0;
}

Slab *cache_select(Cache *cache) {
    assert(cache);
    kinfo("cache select");
        Slab *slab = list_first_entry_or_null(&cache->partial, Slab, list);
        if (slab)
                return slab;
        
        slab = list_first_entry_or_null(&cache->empty, Slab, list);
        if (slab)
                return slab;
        
    if(cache_grow(cache) != 0) return NULL;

    slab = list_first_entry_or_null(&cache->empty, Slab, list);

    kinfo("Selected slab: %p", slab);
    kinfo("Slab memory address: %p", slab->mem);

    return slab;
}

void *cache_alloc(Cache *cache) {
    assert(cache);
    Slab *slab = cache_select(cache);
    if(!slab) return NULL;
    kinfo("Slab memory address: %p", slab->mem);

    size_t index = slab_bufctl(slab)[slab->free];
    if (index >= cache->objects_per_slab) {
        kerror("Index out of bounds: %d", index);
        kpanic("Index out of bounds.");
    }

    
    void *ptr = slab->mem + cache->obj_size * index;
    if (slab->free++ == 0) {
        list_remove(&slab->list);
        list_append(&cache->partial, &slab->list);
    }

    if (slab->free == cache->objects_per_slab) {
        list_remove(&slab->list);
        list_append(&cache->full, &slab->list);
    }

    cache->used++;
    return ptr;
}

bool cache_free_within(Cache *cache, Slab* slab, void* ptr) {
    Slab *first = slab;
    while (slab != (Slab*)first->list.prev) {
        if (ptr >= slab->mem && ptr < slab->mem + cache->obj_size * cache->objects_per_slab) {
            if (slab->free-- == cache->objects_per_slab) {
                list_remove(&slab->list);
                list_append(&cache->partial, &slab->list);
            }
        
            if (slab->free == 0) {
                list_remove(&slab->list);
                list_append(&cache->empty, &slab->list);
            }
            size_t index = (ptr - slab->mem) / cache->obj_size;
            slab_bufctl(slab)[slab->free]=index;
            cache->used--;
            return true;
        }
        slab = (Slab*)slab->list.next;
    }
    return false;
}

void cache_free(Cache *cache, void *ptr) {
    Slab *slab = (Slab*)list_next(&cache->full);
    if(slab) {
        if(!cache_free_within(cache, slab, ptr)) {
            return;
        }
    }

    slab = (Slab*)list_next(&cache->partial);
    if(slab) {
        if(!cache_free_within(cache, slab, ptr)) {
            return;
        }
    }
}

void init_cache(Cache *cache, size_t obj_size) {
    memset(cache, 0, sizeof(cache));
    list_init(&cache->list);
    list_init(&cache->empty);
    list_init(&cache->full);
    list_init(&cache->partial);

    cache->obj_size = obj_size;

    cache->objects_per_slab = (page_align_up(obj_size+sizeof(Slab)+sizeof(uint32_t))-sizeof(Slab)) / (obj_size+sizeof(uint32_t));
}

Cache *cache_create(size_t objsize, const char *name) {
    kinfo("Entered cache create.");
    size_t len = str_len(name);
    if(len >= MAX_CACHE_NAME) {
        kwarning("len is %d, which is bigger than max cache name of %d", len, MAX_CACHE_NAME);
        return NULL;
    }

    kinfo("name checks done.");
    
    assert(kernel.slab_cache);
    kinfo("kernel.slab_cache address: %p", (void*)kernel.slab_cache);

    Cache *cache = (Cache*)cache_alloc(kernel.slab_cache);
    if(!cache) return NULL;
    init_cache(cache, objsize);
    memcpy(&cache->cache_name, name, len + 1);
    list_append(&cache->list, &kernel.caches);
    return cache;
}

void init_slab_allocator() {
    kernel.slab_cache = kernel_malloc(sizeof(Cache));
    init_cache(kernel.slab_cache, sizeof(Cache));
    list_init(&kernel.caches);
} 