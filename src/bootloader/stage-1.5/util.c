#include <stddef.h>
#include <stdint.h>

#include "util.h"

#define UTIL_STR_COLOUR 0x1F
#define UTIL_UINT32_MAX_DIGITS 10

static volatile char *vga_mem = (char *)0xb8000;

int print_cstr(const char *str, uint32_t offset) {
  volatile uint32_t i = offset * 2; // Since each character requires 2 bytes

  while (*str) {
    vga_mem[i++] = *str;
    vga_mem[i++] = UTIL_STR_COLOUR;
    str++;
  }

  return i;
}

void print_uint32(uint32_t n, uint32_t offset) {
  char buf[UTIL_UINT32_MAX_DIGITS + 1];
  const uint32_t buf_size = sizeof(buf);

  buf[buf_size - 1] = 0;
  buf[buf_size - 2] = '0';

  uint32_t i = buf_size - (n ? 1 : 2);
  while (n) {
    i--;
    buf[i] = n % 10 + '0';
    n /= 10;
  }

  print_cstr(&buf[i], offset);
}

static inline char nibble_to_hex(uint8_t nibble) {
  return (nibble < 10) ? nibble + '0' : (nibble - 10) + 'a';
}

void print_uint32_hex(uint32_t n, uint32_t offset) {
  char buf[sizeof(n) * 2 + 1];
  const uint32_t buf_size = sizeof(buf);

  buf[buf_size - 1] = 0;
  buf[buf_size - 2] = '0';

  print_cstr("0x", offset);
  offset += 2;

  uint32_t i = buf_size - (n ? 1 : 2);
  while (n) {
    i--;
    buf[i] = nibble_to_hex(n % 16);
    n /= 16;
  }

  print_cstr(&buf[i], offset);
}

size_t kstrlen(const char *s) {
  size_t i = 0;
  while (i++, *(s++))
    ;
  return i - 1;
}
