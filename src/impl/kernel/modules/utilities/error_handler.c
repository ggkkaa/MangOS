#include <stdint.h>
#include <stdbool.h>
#include "utils.h"
#include "exceptions.h"
#include "print.h"

typedef struct StackFrame {
    struct StackFrame* rbp;
    uintptr_t rip;
} StackFrame;

bool unwind = false;

static void unwind_stack(IDTError* error) {
    unwind = true;
    int depth = 10;
    kllog("+=====STACK TRACE=====+", 1, 4);
    kllog(" %p", 1, 4, (void*)error->rip);
    StackFrame* stack = (StackFrame*)error->rbp;
    while (stack && --depth)
    {
        kllog(" %p", 1, 4, (void*)stack->rip);

        stack = stack->rbp;
    }
    if (depth == 0) kllog(" [...]", 1, 4);
}

void error_handler(IDTError* error) {
    if(error->type == 14) {
        kllog("ERROR PAGE FAULT AT [%p]", 1, 2, error->cr2);
        switch (error->code)
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
    kllog("Register dump: cr2: %p, r15: %p, r14: %p, r13: %p, r12: %p, r11: %p, r10: %p, r9: %p, r8: %p, rbp: %p, rdi: %p, rsi: %p, rdx: %p, rcx: %p, rbx: %p, rax: %p", 1, 2, error->cr2, error->r15, error->r14, error->r13, error->r12, error->r11, error->r10, error->r9, error->r8, error->rbp, error->rdi, error->rsi, error->rdx, error->rcx, error->rbx, error->rax);

    if(!unwind) {
        unwind_stack(error);
    }


    kllog("Error! Error code: %d", 1, 2, error->type);
    kllog("It's at %p btw", 1, 2, error->rip);
    halt();
}

