use16
format binary

org 0x7c00
	
jmp main	

enable_A20:
	; using the bios
	mov ax, 2403h
	int 15h
	jc .bios_failed ; int 15h not supported
	cmp ah, 0
	jnz .bios_failed ; int 15h not supported

	mov ax, 2401h
	int 15h
	jb .bios_failed
	cmp ah, 0
	jnz .bios_failed

	call check_A20
	test ax, ax
	jnz .bios_failed

	ret
	
.bios_failed:
	; Try the other methods
	jmp exit

check_A20:
	pushf
	push si
	push di
	push ds
	push es
	cli

	xor ax, ax
	mov ds, ax
	mov si, 0x7dfe
	mov ax, [si]

	mov bx, 0xffff
	mov ds, bx
	mov si, 0x7e0e

	cmp ax, [si]

	jne .return_success

	xor ax, ax
	mov es, ax
	mov di, 0x7dfe
 	mov [di], WORD 0x6969
	mov ax, [es:di]

	cmp ax, [si]

	jne .return_success

	; Return failure
	mov ax, 1
	jmp .return
	
.return_success:
	xor ax, ax

.return:
	pop es
	pop ds
	pop di
	pop si
	popf
	sti
	ret


main:
	mov sp, stack_end
	mov bp, stack_end
	mov ax, stack_begin
	mov ss, ax
	mov ax, data_begin
	mov ss, ax

	
	call check_A20
	test ax, ax
	jz .A20_enabled

	call enable_A20
	call check_A20
	test ax, ax
	jz .A20_enabled

	jmp exit

.A20_enabled:

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

	jmp exit

; xor cl, cl

; .loop:
; 	push cx
; 
; 	xor ax, ax
; 	mov ds, ax
; 
; 	mov si, cx
; 	mov bx, msg
; 	mov al, [bx + si]
; 	xor bh, bh
; 	mov cx, 1
; 	mov ah, 0ah
; 	int 10h
; 
; 	pop cx
; 	
; 	inc cl
; 
; 	xor bh, bh
; 	mov dh, 0
; 	mov dl, cl
; 	mov ah, 02h
; 	int 10h
; 
; 	cmp cl, len
; 	jl .loop

exit:

data_begin:

stack_begin: 
	rb 200
stack_end:
	
; To make sure binary is within the 440 byte limit
db 'F'

