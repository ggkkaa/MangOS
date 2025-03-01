#define true 1
#include <stddef.h>
#define page_align_up(p)   (((p) + (PAGE_SIZE-1)) / PAGE_SIZE * PAGE_SIZE)
#define page_align_down(p) (((p)                ) / PAGE_SIZE * PAGE_SIZE)

void disable_interrupts();
void enable_interrupts();
void wait_for_interrupt();
void halt();
size_t str_len(const char* str);