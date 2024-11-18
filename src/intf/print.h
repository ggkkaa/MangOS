#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>


void init_serial();
void serial_print_u8(uint8_t c);
void serial_write_str(char* str);
void printf(const char* string, ...)__attribute__((format(printf,1,2)));
