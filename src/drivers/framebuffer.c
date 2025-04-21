#include "drivers/framebuffer.h"
#include "limine/limine.h"
#include "panic.h"
#include "kernel.h"
#include "print.h"
#include "memory/page.h"
#include "memory/linked_list.h"
#include "font.h"

#define BACKGROUND_COLOUR 0x222630
#define FOREGROUND_COLOUR 0xd7dae0

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request limine_framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

void draw_pixel(uint64_t x, uint64_t y, uint32_t color) {
    uint32_t *location = (uint32_t*)(((uint8_t*) kernel.framebuffer.addr) + y * kernel.framebuffer.pitch);
    location[x] = color;
}

void draw_char(char ch, uint64_t x_coord, uint64_t y_coord) {
    uint64_t index = ch * 16;

    // 16 pixel wide character
    for(size_t y = 0; y < 16; y++) {
        // 8 pixel high character
        for(size_t x = 0; x < 8; x++) {
            if ((font[index + y] >> (7 - x)) & 1) {
                draw_pixel(x_coord + x, y_coord + y, FOREGROUND_COLOUR);
            } else {
                draw_pixel(x_coord + x, y_coord + y, BACKGROUND_COLOUR);
            }

        }
    }
}

void print_char(char ch) {
    draw_char(ch, kernel.cursor.x, kernel.cursor.y);
    kernel.cursor.x += 8;
    if (kernel.cursor.x > kernel.framebuffer.width) {
        kernel.cursor.x = 0;
        kernel.cursor.y += 16;
    }
}

void init_framebuffer() {
    if(!limine_framebuffer_request.response) {
        kpanic("Error! No framebuffer found!");
    }

    Framebuffer fb = {
        .addr = (*limine_framebuffer_request.response->framebuffers)->address,
        .width = (*limine_framebuffer_request.response->framebuffers)->width,
        .height = (*limine_framebuffer_request.response->framebuffers)->height,
        .pitch = (*limine_framebuffer_request.response->framebuffers)->pitch,
        .bytes_per_pixel = (*limine_framebuffer_request.response->framebuffers)->bpp / 8,
    };

    kernel.framebuffer = fb;

    size_t framebuffer_size = (size_t)((page_align_up(kernel.framebuffer.height * kernel.framebuffer.pitch)) / PAGE_SIZE);

    page_mmap(kernel.pml4, (paddr_t)kernel.framebuffer.addr - kernel.hhdm, (uintptr_t)kernel.framebuffer.addr, framebuffer_size, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);

    for(size_t y = 0; y < kernel.framebuffer.height; y++) {
        for(size_t x = 0; x < kernel.framebuffer.width; x++) {
            draw_pixel(x, y, BACKGROUND_COLOUR);
        }
    }
}