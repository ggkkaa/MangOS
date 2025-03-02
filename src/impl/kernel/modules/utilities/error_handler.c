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
        halt();
    }
    kllog("Error! Error code: %d", 1, 2, error.type);
    kllog("It's at %p btw", 1, 2, error.rip);
    halt();
}