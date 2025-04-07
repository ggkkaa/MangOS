#include "limine/limine.h"
#include "bootutils.h"
#include "../../../panic.h"

__attribute__((used, section(".limine_requests")))
volatile struct limine_memmap_request limine_memmap_request = {
        .id = LIMINE_MEMMAP_REQUEST,
        .revision = 0,
};



size_t boot_get_memregion_count() {
    if(limine_memmap_request.response) return limine_memmap_request.response->entry_count;
    return 0;
}

void boot_get_memregion_at(BootMemRegion* region, size_t index) {
    if(!limine_memmap_request.response || limine_memmap_request.response->entry_count <= index) kpanic("boot: Trying to get memmap entry at region out of bounds.");

    region->address = limine_memmap_request.response->entries[index]->base;
    region->size = limine_memmap_request.response->entries[index]->length;
    region->kind = limine_memmap_request.response->entries[index]->type;
}