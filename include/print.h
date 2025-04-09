#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#define print_test(num)   kllog('Test %d', 1, 0, num)

enum LOG_TYPE {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_PANIC,
    LOG_TRACE,
};

void init_serial();
void serial_print_u8(uint8_t c);
void serial_write_str(char* str);
void k_serial_printf(const char* string, ...);
void kllog(enum LOG_TYPE log_type, const char* string, ...);

#define kinfo(...) kllog(LOG_INFO, __VA_ARGS__)
#define kwarning(...) kllog(LOG_WARNING, __VAR_ARGS__)
#define kerror(...) kllog(LOG_ERROR, __VA_ARGS__)
#define klpanic(...) kllog(LOG_PANIC, __VA_ARGS__)
#define ktrace(...) kllog(LOG_TRACE, __VA_ARGS__)