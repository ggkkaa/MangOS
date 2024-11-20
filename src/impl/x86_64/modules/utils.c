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
    while (1 == 1)
    {
        wait_for_interrupt();
    }
    
}