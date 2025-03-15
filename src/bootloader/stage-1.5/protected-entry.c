#include <stdint.h>
#include <stddef.h>

#include "test.h"

static char *vga_mem = (char *)0xb8000;

#define STR_COLOUR 0x1F

static void print_cstr(const char *str, uint32_t offset) {
  uint32_t i = offset * 2; // Since each character requires 2 bytes
  while (*str) {
    vga_mem[i++] = *str;
    vga_mem[i++] = STR_COLOUR;
    str++;
  }
}

static void print_int(uint32_t n) {
  //char buf[2] = { 0 };
  char buf[2];
  buf[1] = 0;
  uint32_t i = 0;
  while (n) {
    buf[0] = n % 10 + '0';
    print_cstr(buf, i);
    n /= 10;
    i++;
  }
}

int main(void) {
  print_int(69420);
  for (;;)
    ;
}
