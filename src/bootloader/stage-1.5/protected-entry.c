#include <stdint.h>
#include <stddef.h>

#include "util.h"
#include "idt.h"
#include "interrupts.h"

static struct idt_entry idt_entries[32] = { 0 };
struct idt_desc desc = {
  .limit = sizeof(idt_entries) - 1,
  .offset = (uint32_t)idt_entries,
};

void init_idt(void) {
  for (uint32_t i = 0; i < sizeof(idt_entries) / sizeof(*idt_entries); i++) {
    register_int(idt_entries, i, 0xf, generic_handler);
  }

  register_int(idt_entries, 6, 0xf, invalid_opcode);
  asm volatile("lidt [%[idtr]]\n" : : [idtr] "r"(&desc));
  asm volatile("sti\n" :);
}

int main(void) {
  init_idt();
  volatile int a[16] = { 0 }; // IDT test

  print_cstr("Hello", 80);
  for (;;)
    ;
}
