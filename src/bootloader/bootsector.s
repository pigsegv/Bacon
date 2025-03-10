use16
format binary

org 0x7c00
	
main:
	mov sp, stack_end
	mov bp, stack_end
	
	call check_A20

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
	

enable_A20:
	; using the bios
	mov ax, 2403h
	int 15h
	jc .bios_failed ; int 15h not supported
	cmp ah, 0
	jnz .bios_failed ; int 15h not supported

	; Get A20 status
	mov ax, 2402h
	int 15h
	jb .bios_failed
	cmp ah, 0
	jnz .bios_failed

	; Sanity check to make sure A20 is disabled.
	; We wouldn't even be here if it wasn't.
	cmp al, 1
	je .bios_failed

	mov ax, 2401h
	int 15h
	jb .bios_failed
	cmp ah, 0
	jnz .bios_failed

	ret
	
.bios_failed:
	; Try the other methods
	jmp exit

check_A20:
	xor ax, ax
	mov ds, ax
	mov si, 0x7dfe
	mov ax, [si]

	mov bx, 0xffff
	mov ds, bx
	mov si, 0x7e0e

	cmp ax, [si]

	jne .return

	xor ax, ax
	mov es, ax
	mov di, 0x7dfe
 	mov [di], WORD 0x6969
	mov ax, [es:di]

	cmp ax, [si]

	jne .return

	call enable_A20
	jmp check_A20 ; Test if a20 was enabled (Remove in production)
	
.return:
	ret


exit:

	
msg: db "Hello OS!"
len = $ - msg
	
stack_begin: 
	rb 100
stack_end:
	
; To make sure binary is within the 440 byte limit
db 'F'

