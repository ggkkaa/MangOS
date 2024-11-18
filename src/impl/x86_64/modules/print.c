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

void printf(const char* string, ...) {
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
                serial_write_str(va_arg(args, int));
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

