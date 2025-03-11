use16
format binary

org 0x7c00
	
jmp main	

timer:
	inc WORD [timer_counter]

	cli
	push ax

	mov al, 0x20
	out 0x20, al

	pop ax
	sti

	iret


;print_str:
;	push ds
;
;	xor cx, cx
;.loop:
;	push si
;	push cx
; 
; 	xor ax, ax
; 	mov ds, ax
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

enable_A20:
.bios:
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
.keyboard_controller:
	; TODO

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

macro io_delay {
	xor al, al
	out 0x80, al
}

init_pic:
	cli

	; Set the interrupt handler for the PIT
	xor ax, ax
	mov ds, ax
	mov [PIT_ADDR + 2], ax
	mov ax, timer
	mov [PIT_ADDR], ax

	; ICW1
	mov al, 0x13
	out 0x20, al
	io_delay

	; ICW2
	mov al, PIT_INT
	out 0x21, al
	io_delay

	; ICW3
	mov al, 0
	out 0x21, al
	io_delay

	; ICW4
	mov al, 0x01
	out 0x21, al
	io_delay

	; Mask all but IRQ0
	mov al, 0xfe
	out 0x21, al
	
	sti

	ret

init_pit:
	mov al, 0x36
	out 0x43, al

	mov ax, 0x4aa
	out 0x40, al
	mov al, ah
	out 0x40, al

	ret
	

main:
	mov sp, 0xffff
	mov bp, 0xffff
	mov ax, 0x7000
	mov ss, ax

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

	call init_pic
	call init_pit
	
	call check_A20
	test ax, ax
	jz .A20_enabled

	call enable_A20
	call check_A20
	test ax, ax
	jz .A20_enabled

	jmp exit

.A20_enabled:
	xor ax, ax
	mov ds, ax

.loop:
	mov ax, [timer_counter]
	cmp ax, 1000

	jnz .loop

 	mov al, [char]
 	xor bh, bh
 	mov cx, 10
 	mov ah, 0ah
 	int 10h
	
	inc BYTE [char]
	mov WORD [timer_counter], 0

	jmp .loop


	jmp exit

exit:

data_begin:

;scratch_qword: rq 1

char: db '1'
timer_counter: dw 0


PIT_INT = 0x20
PIT_ADDR = 4 * PIT_INT
	
; To make sure binary is within the 440 byte limit
db 'F'

