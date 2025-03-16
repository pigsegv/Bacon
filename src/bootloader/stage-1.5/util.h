#ifndef BOOT_UTIL_H
#define BOOT_UTIL_H

#include <stdint.h>

int print_cstr(const char *str, uint32_t offset);
void print_uint32(uint32_t n, uint32_t offset);

#endif // BOOT_UTIL_H
