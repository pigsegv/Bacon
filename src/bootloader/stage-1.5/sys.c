#include "sys.h"

void kabort(void) {
  asm volatile("cli\n"
               "hlt\n");
}
