#ifndef BOOT_IDT_H
#define BOOT_IDT_H

#include <stdint.h>

struct idt_desc {
  uint16_t limit;
  uint32_t offset;
} __attribute__((packed));

struct idt_entry {
  uint16_t offset_low;
  uint16_t segment;

  uint8_t reserved;

  uint8_t gate_type : 4;
  uint8_t : 1;
  uint8_t privilege : 2;
  uint8_t present : 1;

  uint16_t offset_high;
} __attribute__((packed));

void register_int(struct idt_entry *entries, uint32_t entry, uint8_t gate_type,
                  void (*handler)());

#endif // BOOT_IDT_H
