use16
format binary

CODE_BEGIN_ADDR = 0x7e00

org 0x0

jmp main

include 'a20.s'
include 'protected.s'

; prints number inside eax
print_num:
	xor cx, cx
	mov ebx, 10

.loop:
	xor edx, edx
	div ebx
	add dl, '0'
	shl dx, 8 ; little-endian
	push dx
	add sp, 1
	inc cx

	test eax, eax
	jnz .loop

	mov bx, sp

	push di
	push si
	push cx

	mov di, bx
	mov si, cx
	mov bx, ss
	call print_str

	pop cx
	pop si
	pop di

	add sp, cx

	ret

; bx -> string segment
; di -> string offset
; si -> length
print_str:
	push ds

	xor cx, cx

 	mov ds, bx

.loop:
	push si
	push cx

 	xor bh, bh
 	mov dh, 0
 	mov dl, cl
 	mov ah, 02h
 	int 10h
 
 
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
 
 	cmp cx, si
 	jl .loop

	pop ds

	ret

main:
	mov ax, 0x7e0
	mov ds, ax

	call init_pic
	call init_pit

	call enable_A20

	call check_A20
	test ax, ax
	jnz exit

	jmp to_protected

	mov WORD [timer_counter], 1000
.loop:
	cmp WORD [timer_counter], 1000
	jl .loop

	mov eax, [counter]
	call print_num

	mov WORD [timer_counter], 0
	inc DWORD [counter]
	jmp .loop


	jmp exit
	
exit:
	mov di, failed
	mov si, failed.len
	mov bx, ds
	call print_str
.loop:
	jmp .loop

struc string [data] {
	. db data
	.len = $ - .
}

failed string "Failed to boot."

counter: dd 0

use32
protected_mode_start:
	jmp protected_mode_start
	db "Protected mode starts here!"
