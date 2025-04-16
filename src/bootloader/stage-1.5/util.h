#ifndef BOOT_UTIL_H
#define BOOT_UTIL_H

#include <stdint.h>
#include <stddef.h>

int print_cstr(const char *str, uint32_t offset);
int print_uint32(uint32_t n, uint32_t offset);
int print_uint32_hex(uint32_t n, uint32_t offset);

size_t kstrlen(const char *s);

#endif // BOOT_UTIL_H
