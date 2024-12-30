/* MIT License
*
* Copyright (c) 2024 ggkkaa
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "print.h"
#include "strconvert.h"

#define COM_PORT 0x3f8
#define COM_5 (COM_PORT+5)
#define COM_STATUS COM_5

#define COM_LINE_CONTROL_PORT (COM_PORT+3)
#define COM_FIFO_CONTROL_PORT (COM_PORT+2)
#define COM_INT_ENABLE_PORT   (COM_PORT+1)


static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

void init_serial() {
    // Set baud rate (for example, 9600 baud)
    outb(COM_LINE_CONTROL_PORT, 0x80); // Enable DLAB (divisor latch access)
    outb(COM_PORT, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(COM_INT_ENABLE_PORT, 0x00); // Hi byte
    outb(COM_LINE_CONTROL_PORT, 0x03); // 8 bits, no parity, one stop bit
    outb(COM_FIFO_CONTROL_PORT, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(COM_INT_ENABLE_PORT, 0x00); // Enable received data available interrupt
}

void serial_print_u8(uint8_t c) {
    while ((inb(COM_STATUS) & 0x20) == 0) {}
    outb(COM_PORT, c);
}

void serial_write_str(char* str) {
    for (size_t i = 0; 1; i++) {
        char character = (uint8_t) str[i];

        if(character == '\0') {
            return;
        }

        serial_print_u8(character);
    }
}

void kprintf(const char* string, ...) {
    va_list args;
    va_start(args, string);
    for (const char* ptr = string; *ptr != '\0'; ptr++) {
        if(*ptr == '%') {
            ptr++;
            switch (*ptr)
            {
            case 'd':
                serial_write_str(int_to_str(va_arg(args, int)));
                break;
            case 's':
                serial_write_str(va_arg(args, char*));
                break;
            case 'p':
                serial_write_str(ptr_to_str(va_arg(args, void*)));
                break;
            default:
                break;
            }
        } else {
            serial_print_u8(*ptr);
        }
    }
    va_end(args);
}

void v_kprintf(const char* string, va_list args) {
    for (const char* ptr = string; *ptr != '\0'; ptr++) {
        if(*ptr == '%') {
            ptr++;
            switch (*ptr)
            {
            case 'd':
                serial_write_str(int_to_str(va_arg(args, int)));
                break;
            case 's':
                serial_write_str(va_arg(args, char*));
                break;
            case 'p':
                serial_write_str(ptr_to_str(va_arg(args, void*)));
                break;
            default:
                break;
            }
        } else {
            serial_print_u8(*ptr);
        }
    }
}

void kllog(const char* string, uint8_t end_line, uint8_t log_type, ...) {
    va_list args;
    va_start(args, string);
    switch (log_type)
    {
    case 0:
        kprintf("[INFO]          ");
        break;
    case 1:
        kprintf("\033[33m[WARNING]       ");
        break;
    case 2:
        kprintf("\033[31m[ERROR]         ");
        break;
    default:
        break;
    }

    v_kprintf(string, args);
    kprintf("\033[0m");
    
    switch (end_line)
    {
    case 0:
        break;
    case 1:
        kprintf("\n");
        break;
    default:
        kprintf("\n");
        break;
    }

    va_end(args);
}