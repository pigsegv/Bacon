#include <stddef.h>
#include <stdint.h>

#include "interrupts.h"
#include "util.h"

#define INTERRUPTS_CODE_SEGMENT ((uint16_t)1)

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

void register_int(struct idt_entry *entries, uint32_t entry, uint8_t gate_type,
                  void (*handler)()) {
  struct idt_entry *e = &entries[entry];
  e->reserved = 0;

  e->present = 1;
  asm volatile(
    "mov eax, %[ptr]\n"
    "mov %[offset_low], ax\n"
    "shr eax, 16\n"
    "mov %[offset_high], ax\n"
    : [offset_low] "=r"(e->offset_low), [offset_high] "=r"(e->offset_high)
    : [ptr] "r"(handler)
    : "eax", "ax");
  e->segment = 0 | (INTERRUPTS_CODE_SEGMENT << 3);
  e->gate_type = gate_type;
  e->privilege = 0;
}
