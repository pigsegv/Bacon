/* "phys" gets appended when this script is called */

OUTPUT_FORMAT("binary")
OUTPUT_ARCH(i386)

ENTRY(main)
SECTIONS
{
  .text.main phys : AT(phys) {
    main = .;
    *(.text.main)
  }
  .text : AT(phys + (code - main)) {
    code = .;
    *(.text)
    *(.rodata)
  }
  .data : AT(phys + (data - main))
  {
    data = .;
    *(.data)
  }
  .bss : AT(phys + (bss - main))
  {
    bss = .;
    *(.bss)
  }
  end = .;
}
