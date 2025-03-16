/* "phys" gets appended when this script is called */

OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)
ENTRY(main)

SECTIONS
{
  .text phys : AT(phys) {
    code = .;
		*(.text.*.main)
    *(.text*)
    *(.rodata)
  }

  .bss : AT(phys + (bss - code))
  {
    bss = .;
    *(.bss)
  }

  .data : AT(phys + (data - code))
  {
    data = .;
    *(.data)
  }
}
