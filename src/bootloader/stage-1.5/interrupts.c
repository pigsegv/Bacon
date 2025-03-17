#include <stddef.h>
#include <stdint.h>

#include "util.h"

__attribute__((used)) static void print_msg(uint32_t addr) {
  const char *msg = "Invalid opcode at ";
  print_cstr(msg, 0);
  print_uint32_hex(addr - 0x7e00, kstrlen(msg));
}

__attribute__((naked)) void invalid_opcode() {
  asm volatile("call print_msg\n");

  asm volatile("cli\n"
               "hlt\n"
               :);
}

__attribute__((noreturn)) void generic_handler(void) {
  print_cstr("EXCEPTION!     ", 0);
  asm volatile("cli\n"
               "hlt\n"
               :);
  for (;;)
    ;
}
