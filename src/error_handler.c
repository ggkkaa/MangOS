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
    ktrace("+=====STACK TRACE=====+");
    ktrace(" %p", (void*)error->rip);
    StackFrame* stack = (StackFrame*)error->rbp;
    while (stack && --depth)
    {
        ktrace(" %p", (void*)stack->rip);

        stack = stack->rbp;
    }
    if (depth == 0) ktrace(" [...]");
}

void error_handler(IDTError* error) {
    if(error->type == 14) {
        kerror("ERROR PAGE FAULT AT [%p]", error->cr2);
        switch (error->code)
        {
        case 0:
            kerror("Page not present");
            break;
        case 1:
            kerror("Protection fault");
            break;
        case 2:
            kerror("Write fault");
            break;
        case 3: 
            kerror("User mode access");
            break;
        default: 
            kerror("Unknown error");
            break;
        }

    }
    kerror("Register dump: cr2: %x, r15: %x, r14: %x, r13: %x, r12: %x, r11: %x, r10: %x, r9: %x, r8: %x, rbp: %x, rdi: %x, rsi: %x, rdx: %x, rcx: %x, rbx: %x, rax: %x",
        error->cr2, error->r15, error->r14, error->r13, error->r12, error->r11, error->r10, error->r9,
        error->r8, error->rbp, error->rdi, error->rsi, error->rdx, error->rcx, error->rbx, error->rax);

    if(!unwind) {
        unwind_stack(error);
    }


    kerror("Error! Error code: %d", error->type);
    kerror("It's at %p btw", error->rip);
    halt();
}

