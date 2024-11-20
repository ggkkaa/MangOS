#include <stdint.h>
#include "utils.h"

void error_handler(uint64_t exit_code) {
    printf("Error! Error code: %d\n", exit_code);
    halt();
}