#pragma once

#include <stdint.h>

typedef struct
{
        void *addr;
        uint64_t width;
        uint64_t height;
        uint64_t pitch;
        uint64_t bytes_per_pixel;
} Framebuffer;

typedef struct
{
        uint64_t x;
        uint64_t y;
} Cursor;


void init_framebuffer();
void draw_char(char ch, uint64_t x_coord, uint64_t y_coord);