use16
format binary

org 0x7e00


timer:
	inc WORD [timer_counter]

	cli
	push ax

	mov al, 0x20
	out 0x20, al

	pop ax
	sti

	iret

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
	mov WORD [timer_counter], 0
	call .wait_input_status


.wait_input_status:
	cmp WORD [timer_counter], 5000
	jae exit

	in al, 0x64
	test al, 0x02

	jnz .wait_input_status
	
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

exit:
	; Error msg
.loop:
	jmp .loop



timer_counter: dw 0


PIT_INT = 0x20
PIT_ADDR = 4 * PIT_INT
