#include <stdint.h>
#include "utils.h"

void error_handler(uint64_t exit_code) {
    if(exit_code == 14) {
        kllog("ERROR PAGE FAULT", 1, 2);
        halt();
    }
    kllog("Error! Error code: %d", 1, 2, exit_code);
    halt();
}