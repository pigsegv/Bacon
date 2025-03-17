#include "idt.h"

#define CODE_SEGMENT ((uint16_t)1)

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
  e->segment = 0 | (CODE_SEGMENT << 3);
  e->gate_type = gate_type;
  e->privilege = 0;
}
