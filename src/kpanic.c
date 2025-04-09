#include "print.h"
#include "panic.h"
#include "utils.h"

void kpanic(char* message) {
    klpanic("The mango has rotten!");
    klpanic("Error message: %s", message);
    halt();
}