#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>


void init_serial();
void serial_print_u8(uint8_t c);
void serial_write_str(char* str);
void k_serial_printf(const char* string, ...)__attribute__((format(printf,1,2)));
void kllog(const char* string, uint8_t end_line, uint32_t log_type, ...);