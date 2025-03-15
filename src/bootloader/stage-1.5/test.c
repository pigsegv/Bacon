#include "test.h"

void test() {
  char *vga_mem = (char *)0xb8000;
  vga_mem[0] = 'H';
  vga_mem[1] = 0x1f;
  vga_mem[2] = 'I';
  vga_mem[3] = 0x1f;
  vga_mem[4] = ' ';
  vga_mem[5] = 0x1f;
  vga_mem[6] = 'C';
  vga_mem[7] = 0x1f;
  vga_mem[8] = '!';
  vga_mem[9] = 0x1f;
}
