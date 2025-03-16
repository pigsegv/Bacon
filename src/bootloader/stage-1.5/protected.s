use16

to_protected:
	xor eax, eax
	mov ax, ds
	shl eax, 4
	add eax, gdt
	mov [gdt_descriptor.offset], eax

	cli
	; mask pic interrupts
	mov al, 0xff
	out 0x21, al

	lgdt [gdt_descriptor]
	mov eax, cr0 
	or al, 1
	mov cr0, eax
	
	mov ax, gdt.data_offset
	mov ds, ax
	mov ss, ax
	mov es, ax
	jmp gdt.text_offset:(CODE_BEGIN_ADDR + protected_mode_start)

; 32 bit gdt descriptor
gdt_descriptor:
	.size: dw gdt.len - 1
	.offset: rd 1

gdt: 
	.null: dq 0
	.kernel_text:
		dw 0xffff
		dw 0
		db 0
		db 0x9b
		db 0xcf
		db 0
	.kernel_data:
		dw 0xffff
		dw 0
		db 0
		db 0x93
		db 0xcf
		db 0
	.len = $ - gdt
	.text_offset = .kernel_text - gdt
	.data_offset = .kernel_data - gdt

