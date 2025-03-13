use16
format binary

org 0x7c00
	
jmp main	

; prints number inside eax
;print_num:
;	xor cx, cx
;	mov ebx, 10
;
;.loop:
;	xor edx, edx
;	div ebx
;	add dl, '0'
;	shl dx, 8 ; little-endian
;	push dx
;	add sp, 1
;	inc cx
;
;	test eax, eax
;	jnz .loop
;
;	mov bx, sp
;
;	push di
;	push si
;	push cx
;
;	mov di, bx
;	mov si, cx
;	mov bx, ss
;	call print_str
;
;	pop cx
;	pop si
;	pop di
;
;	add sp, cx
;
;	ret
;
;; bx -> string segment
;; di -> string offset
;; si -> length
;print_str:
;	push ds
;
;	xor cx, cx
;
; 	mov ds, bx
;
;.loop:
;	push si
;	push cx
; 
; 
; 	mov si, cx
; 	mov bx, di
; 	mov al, [bx + si]
; 	xor bh, bh
; 	mov cx, 1
; 	mov ah, 0ah
; 	int 10h
; 
; 	pop cx
;	pop si
; 	
; 	inc cl
; 
; 	xor bh, bh
; 	mov dh, 0
; 	mov dl, cl
; 	mov ah, 02h
; 	int 10h
; 
; 	cmp cx, si
; 	jl .loop
;
;	pop ds
;
;	ret

FIRST_PARTITION_ENTRY = 0x7c00 + 0x1be

; Parses mbr and stores the space between it and the first partition in eax
; Assumes that ds is zero
parse_mbr:
	mov cx, 4
	mov si, FIRST_PARTITION_ENTRY
	mov ebx, 0xffffffff ; Min starting block
	xor dl, dl ; (valid partition exits) ? 1 : 0
.loop_entries:
	mov eax, [si + 0x0c] ; Number of blocks in sector
	test eax, eax
	jz .loop.continue

	mov dl, 1

	cmp [si + 0x08], ebx
	cmovb ebx, [si + 0x08]

	test ebx, ebx
	jz exit ; Invalid partition table
	
.loop.continue:
	add si, 16
	loop .loop_entries

.loop.break:
	test dl, dl
	jz exit ; No valid partition exists

	sub ebx, 1
	test ebx, ebx
	jz exit ; Partition starts right after mbr (bootloader cannot exist)

	mov eax, ebx

	ret

MAX_SECTORS_TO_LOAD = 127
LOAD_SECTORS_TO_SEG = 0x7e0
LOAD_SECTORS_TO_OFF = 0

main:
	xor ax, ax
	mov ds, ax

	mov [bios_device_id], dl

	mov sp, 0xffff
	mov bp, 0xffff
	mov ax, 0x7000
	mov ss, ax

	; TODO: Query disk geometry to confirm 512 byte sectors

	xor al, al
	mov ah, 05h
	int 10h

	mov al, 03h
	mov ah, 00h
	int 10h

	xor bh, bh
	mov dh, 0
	mov dl, 0
	mov ah, 02h
	int 10h

	call parse_mbr

	mov ebx, MAX_SECTORS_TO_LOAD
	
	cmp eax, MAX_SECTORS_TO_LOAD
	cmova eax, ebx

	mov [disk_packet.num_sectors], ax
	mov WORD [disk_packet.buffer_offset], LOAD_SECTORS_TO_OFF
	mov WORD [disk_packet.buffer_segment], LOAD_SECTORS_TO_SEG
	mov DWORD [disk_packet.lba_low], 1

	; Check if disk extensions are present
	mov dl, [bios_device_id]
	mov bx, 0x55aa
	mov ah, 41h
	int 13h
	
	jc exit
	cmp bx, 0xaa55
	jne exit

	test cx, 1b
	jz exit

.extensions_present:
	mov dl, [bios_device_id]
	mov si, disk_packet
	mov ah, 42h
	int 13h
	jc exit

	jmp LOAD_SECTORS_TO_SEG:LOAD_SECTORS_TO_OFF

	jmp exit

exit:
	jmp exit ; Infinite loop

data_begin:
bios_device_id: rb 1

align 2
disk_packet:
	.size: db 16
	.reserved: db 0
	.num_sectors: rb 2
	.buffer_offset: rb 2
	.buffer_segment: rb 2
	.lba_low: rb 4
	.lba_high: dd 0


greeter: db "Yo, wassup!"
	.len = $ - greeter


	
; To make sure binary is within the 440 byte limit
db 'F'

