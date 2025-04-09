#include <stdint.h>
#include <stddef.h>

char* bin_to_str(uint64_t num);
char* hex_to_str(uint64_t num);
char* ptr_to_str(void* ptr);
char* uint32_to_str(uint32_t uint32);
char* int_to_str(int integer);
const char* strflip(char* str, size_t len);
size_t itoa(char* buf, size_t cap, int value);
size_t uptrtoha_full(char* buf, size_t cap, uintptr_t value, const char* digits);