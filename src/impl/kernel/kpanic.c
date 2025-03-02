#include "print.h"
#include "panic.h"
#include "modules/utilities/include/utils.h"

void kpanic(char* message) {
    kllog("The mango has rotten!", 1, 3);
    kllog("Error message: %s", 1, 3, message);
    halt();
}