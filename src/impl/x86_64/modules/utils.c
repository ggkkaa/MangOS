#include "utils.h"


void disable_interrupts(){
    asm("cli");
}
void enable_interrupts() {
    asm("sti");
}
void wait_for_interrupt() {
    asm("hlt");
}
void halt() {
    disable_interrupts();
    while (true)
    {
        wait_for_interrupt();
    }
    
}

size_t str_len(const char* str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}