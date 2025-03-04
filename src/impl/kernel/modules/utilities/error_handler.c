#include <stdint.h>
#include "utils.h"
#include "exceptions.h"
#include "print.h"

void error_handler(IDTError error) {
    if(error.type == 14) {
        kllog("ERROR PAGE FAULT AT [%p]", 1, 2, error.cr2);
        switch (error.code)
        {
        case 0:
            kllog("Page not present", 1, 2);
            break;
        case 1:
            kllog("Protection fault", 1, 2);
            break;
        case 2:
            kllog("Write fault", 1, 2);
            break;
        case 3: 
            kllog("User mode access", 1, 2);
            break;
        default: 
            kllog("Unknown error", 1, 2);
            break;
        }

    }
    kllog("Register dump: cr2: %d, r15: %d, r14: %d, r13: %d, r12: %d, r11: %d, r10: %d, r9: %d, r8: %d, rbp: %d, rdi: %d, rsi: %d, rdx: %d, rcx: %d, rbx: %d, rax: %d", 1, 2, error.cr2, error.r15, error.r14, error.r13, error.r12, error.r11, error.r10, error.r9, error.r8, error.rbp, error.rdi, error.rsi, error.rdx, error.rcx, error.rbx, error.rax);

    kllog("Error! Error code: %d", 1, 2, error.type);
    kllog("It's at %p btw", 1, 2, error.rip);
    halt();
}

