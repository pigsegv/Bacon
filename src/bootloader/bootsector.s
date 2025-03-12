use16
format binary

org 0x7c00
	
jmp main	

print_num:
	; TODO

print_str:
	push ds

	xor cx, cx
.loop:
	push si
	push cx
 
 	xor ax, ax
 	mov ds, ax
 
 	mov si, cx
 	mov bx, di
 	mov al, [bx + si]
 	xor bh, bh
 	mov cx, 1
 	mov ah, 0ah
 	int 10h
 
 	pop cx
	pop si
 	
 	inc cl
 
 	xor bh, bh
 	mov dh, 0
 	mov dl, cl
 	mov ah, 02h
 	int 10h
 
 	cmp cx, si
 	jl .loop

	pop ds

	ret

FIRST_PARTITION_ENTRY = 0x7c00 + 0x1be

; Parses mbr and stores the space between it and the first partition in eax
; Assumes that ds is zero
parse_mbr:
	mov cx, 4
	mov si, FIRST_PARTITION_ENTRY
	mov ebx, 0xffffffff ; Min starting block
	xor dl, dl ; (valid partition exits) ? 1 : 0
.loop:
	mov eax, [si + 0x0c] ; Number of blocks in sector
	test eax, eax
	jz .continue

	mov dl, 1

	cmp ebx, [si + 0x08]
	cmovb ebx, [si + 0x08]

	test ebx, ebx
	jz exit ; Invalid partition table
	
.continue:
	add si, 16
	loop .loop

.break:
	test dl, dl
	jz exit ; No valid partition exists

	sub ebx, 1
	test ebx, ebx
	jz exit ; Partition starts right after mbr (bootloader cannot exist)

	mov eax, ebx

	ret

main:
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

;	cmp eax, 63
;	jne exit
	mov [temp], eax

	push eax
	mov di, temp
	mov si, 4
	call print_str
	pop eax

	jmp exit

exit:
	jmp exit ; Infinite loop

data_begin:

greeter: db "Yo, wassup!"
	.len = $ - greeter

;scratch_qword: rq 1
temp: rd 1

char: db '1'
	
; To make sure binary is within the 440 byte limit
db 'F'

