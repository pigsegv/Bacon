#include <stdint.h>
#include "test.h"

struct test {
  char a;
  char b;
};

struct test t = {
  .a = 'A',
  .b = 'B',
};

int main(void) {
  test();

  // ;char *vga_mem = (char *)0xb8000;
  // ;vga_mem[0] = t.b;
  // ;vga_mem[1] = 0x1f;
  for (;;)
    ;
}
