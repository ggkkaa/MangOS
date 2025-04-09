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

#include "strconvert.h"

char* uint32_to_str(uint32_t uint32) {
    static char buffer[11]; 
    
    size_t index = 10;
    buffer[index] = '\0';

    // If 0, return '0'
    if (uint32 == 0) {
        buffer[--index] = '0';
        return &buffer[index];
    }
    // Convert the number to a string
    while (uint32 > 0) {
        uint32_t digit = uint32 % 10;
        buffer[--index] = '0' + digit;
        uint32 = uint32 / 10;
    }

    return &buffer[index];
}

char* int_to_str(int integer) {
    static char buffer[11]; 
    
    size_t index = 10;
    buffer[index] = '\0';

    // If 0, return '0'
    if (integer == 0) {
        buffer[--index] = '0';
        return &buffer[index];
    }
    // Convert the number to a string
    while (integer > 0) {
        uint32_t digit = integer % 10;
        buffer[--index] = '0' + digit;
        integer = integer / 10;
    }

    return &buffer[index];
}


const char* strflip(char* str, size_t len) {
    for(size_t i = 0; i < len/2; ++i) {
        char c = str[i];
        str[i] = str[len-i-1];
        str[len-i-1] = c;
    }
    return str;
}
size_t itoa(char* buf, size_t cap, int value) {
    size_t at=0;
    if(value == 0 && at < cap) {
        buf[at++] = '0';
        return at;
    }

    if(value < 0 && at < cap) {
        buf[at++] = '-';
        value = -value;
    }
    char* whole = buf+at;
    while(at < cap && value > 0) {
        int k = value % 10;
        value /= 10;
        buf[at++] = '0'+k;
    }
    strflip(whole, (buf+at)-whole);
    return at;
}
const char* hex_upper_digits = "0123456789ABCDEF";
size_t uptrtoha_full(char* buf, size_t cap, uintptr_t value, const char* digits) {
    size_t at=0;
    while(at < cap && at < 16) {
        int k = value & 0xF;
        value >>= 4;
        buf[at++] = digits[k];
    }
    strflip(buf, at);
    return at;
}

char* ptr_to_str(void* ptr) {
    static char buffer[20]; 
    unsigned long long addr = (unsigned long long)ptr;
    const char hex_digits[] = "0123456789ABCDEF";

    buffer[0] = '0';  // Prefix for hexadecimal
    buffer[1] = 'x';  // Prefix for hexadecimal

    // Convert each the pointer to hex
    for (int i = (sizeof(addr) * 2) - 1; i >= 0; --i) {
        buffer[2 + i] = hex_digits[addr & 0xF];
        addr >>= 4;                            
    }
    buffer[2 + (sizeof(addr) * 2)] = '\0'; 
    return buffer;
}

char* hex_to_str(uint64_t num) {
    static char buffer[20]; 
    unsigned long long addr = (unsigned long long)num;
    const char hex_digits[] = "0123456789ABCDEF";

    buffer[0] = '0';  // Prefix for hexadecimal
    buffer[1] = 'x';  // Prefix for hexadecimal

    // Convert each the pointer to hex
    for (int i = (sizeof(addr) * 2) - 1; i >= 0; --i) {
        buffer[2 + i] = hex_digits[num & 0xF];
        addr >>= 4;                            
    }
    buffer[2 + (sizeof(addr) * 2)] = '\0'; 
    return buffer;
}

char* bin_to_str(uint64_t num) {
    static char buffer[66]; 
    unsigned long long addr = (unsigned long long)num;
    const char hex_digits[] = "01";

    buffer[0] = '0';  // Prefix for hexadecimal
    buffer[1] = 'b';  // Prefix for hexadecimal

    // Convert each the pointer to hex
    for (int i = (sizeof(addr) * 2) - 1; i >= 0; --i) {
        buffer[2 + i] = hex_digits[num & 0xF];
        addr >>= 4;                            
    }
    buffer[2 + (sizeof(addr) * 2)] = '\0'; 
    return buffer;
}