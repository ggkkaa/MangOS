#include "memory/memory.h"
#include "memory/linked_list.h"
#include "kernel.h"
#include "utils.h"
#include "panic.h"
#include "print.h"

void *kernel_malloc(size_t size) {
    void* addr = (void*)alloc_phys_pages(round_to_page(size));
    if(!addr) {
        kpanic("Malloc: No memory left.");
    }
    kinfo("Allocated %d bytes at %p physical address.", size, addr);
    addr = (void*)((uintptr_t)addr + kernel.hhdm);
    kinfo("Allocated %d bytes at %p", size, addr);
    return addr;
}