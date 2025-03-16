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
  char buf[UTIL_UINT32_MAX_DIGITS + 1] = { 0 };
  buf[UTIL_UINT32_MAX_DIGITS - 2] = '0';

  uint32_t i = UTIL_UINT32_MAX_DIGITS - 1;
  while (n) {
    i--;
    buf[i] = n % 10 + '0';
    n /= 10;
  }

  print_cstr(&buf[i], offset);
}
