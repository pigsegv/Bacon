use16

TABLE_START_SEG = 0xfc0
TABLE_START_OFF = 0x0
ENTRY_SIZE = 24

load_mmap:
	push es
	push di

	mov ax, TABLE_START_SEG
	mov es, ax
	mov di, TABLE_START_OFF
	
	mov DWORD [es:di + 0], 0 ; .count

	add di, 5 ; .entries

	mov DWORD [es:di + 20], 1 ; for ACPI compatibility

	xor ebx, ebx
	mov edx, 0x534D4150 ; Magic number
	mov ecx, 24
	mov eax, 0xe820
	int 15h

	mov [es:di - 1], cl ; .entry_size

	cmp eax, 0x534D4150
	jne exit
	jc exit
	test ebx, ebx
	jz exit

.loop:
	add di, ENTRY_SIZE
	inc DWORD [mmap_count]

	mov DWORD [es:di + 20], 1 ; for ACPI compatibility

	mov edx, 0x534D4150
	mov ecx, 24
	mov eax, 0xe820
	int 15h

	jc .done
	test ebx, ebx
	jz .done

	jmp .loop

.done:
	xor di, di
	mov DWORD [es:di + 0], mmap_count

	mov eax, [es:di]
	call print_num

	pop di
	pop es
	ret

mmap_count: dd 0


; mmap table layout:
; mmap:
;		.count: rd 1
;		.entry_size: rb 1
;		.entries:
;		...
