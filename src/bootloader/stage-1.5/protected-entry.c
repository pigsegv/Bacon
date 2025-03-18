#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "util.h"
#include "interrupts.h"

static struct idt_entry idt_entries[32] = { 0 };
struct idt_desc desc = {
  .limit = sizeof(idt_entries) - 1,
  .offset = (uint32_t)idt_entries,
};

int main(void) {
  // init IDT
  {
    for (uint32_t i = 0; i < sizeof(idt_entries) / sizeof(*idt_entries); i++) {
      register_int(idt_entries, i, 0xf, generic_handler);
    }

    register_int(idt_entries, 6, 0xf, invalid_opcode);

    // TODO: Rework interrupts API to be less clunky

    asm volatile("lidt [%[idtr]]\n" : : [idtr] "r"(&desc));
    asm volatile("sti\n" :);
  }

  [[maybe_unused]] volatile int a[16] = { 0 }; // IDT test

  print_cstr("Hello", 80);
  for (;;)
    ;
}
