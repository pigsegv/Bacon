#include <stdint.h>
#include <stddef.h>

#include "util.h"
#include "idt.h"

static struct idt_entry idt_entries[31] = { 0 };
struct idt_desc desc = {
  .limit = sizeof(idt_entries) - 1,
  .offset = (uint32_t)idt_entries,
};

__attribute__((noreturn)) void invalid_opcode(void) {
  print_cstr("EXCEPTION!     ", 0);
  asm volatile("cli\n"
               "hlt\n"
               :);
  for (;;)
    ;
}

void init_idt(void) {
  register_int(idt_entries, 0, 0xf, invalid_opcode);
  asm volatile("lidt [%[idtr]]\n" : : [idtr] "m"(desc));
  asm volatile("sti\n" :);
}

int main(void) {
  init_idt();
  volatile uint32_t a = 5 / 0;

  print_cstr("Hello", 80);
  for (;;)
    ;
}
