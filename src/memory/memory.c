#include "memory/memory.h"
#include "memory/linked_list.h"
#include "kernel.h"
#include "utils.h"
#include "panic.h"

void *kernel_malloc(size_t size) {
    void* addr = (void*)alloc_phys_pages(round_to_page(size));
    if(!addr) {
        kpanic("Malloc: No memory left.");
    }

    addr = (void*)((uintptr_t)addr + kernel.hhdm);
    return addr;
}