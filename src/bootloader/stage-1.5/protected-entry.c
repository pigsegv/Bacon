#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "util.h"
#include "interrupts.h"

struct mmap_entry {
  uint64_t base;
  uint64_t length;

  uint32_t type;

  uint32_t acpi_attr;
} __attribute__((packed));

struct mmap {
  uint32_t count;
  uint8_t entry_size;

  struct mmap_entry entries[];
} __attribute__((packed));

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

  struct mmap *mmap = (struct mmap *)0xfc00;
  print_uint32(mmap->count * sizeof(struct mmap_entry), 0);

  print_cstr("Hello", 160);

  [[maybe_unused]] volatile int a[16] = { 0 };

  print_cstr("Hi", 80);

  asm volatile("cli\n"
               "hlt\n");
}
