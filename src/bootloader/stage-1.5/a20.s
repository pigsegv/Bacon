use16


PIT_INT = 0x20
PIT_ADDR = 4 * PIT_INT
TIMEOUT_MS = 5000

timer:
	inc WORD [timer_counter]

	cli
	push ax

	mov al, 0x20
	out 0x20, al

	pop ax
	sti

	iret

macro wait_input {
	mov WORD [timer_counter], 0
	sti
	call .wait_input_status
	cli
}

macro wait_output {
	mov WORD [timer_counter], 0
	sti
	call .wait_output_status
	cli
}

macro wait_a20 {
	mov WORD [timer_counter], 0
	sti
	call .wait_a20_status
	cli
}

enable_A20:
	cli

	call check_A20
	test ax, ax
	jz .exit ; already enabled

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

	jmp .exit
	
.bios_failed:
.keyboard_controller:
	wait_input
	mov al, 0xad ; Disable first ps/2
	out 0x64, al

	wait_input
	mov al, 0xd0
	out 0x64, al

	wait_output
	in al, 0x60
	push eax

	wait_input
	mov al, 0xd1
	out 0x64, al

	wait_input
	pop eax
	or al, 2 ; Enable a20
	out 0x60, al

	wait_input
	mov al, 0xae ; Enable first ps/2
	out 0x64, al

	wait_a20
	test ax, ax
	jnz .keyboard_controller_failed

	jmp .exit

.keyboard_controller_failed:
.fast_a20:
	in al, 0x92
	test al, 2
	jnz .exit
	or al, 2
	and al, 0xfe
	out 0x92, al

	wait_a20

	jmp .exit

.wait_input_status:
	cmp WORD [timer_counter], TIMEOUT_MS
	jae .keyboard_controller_failed

	in al, 0x64
	test al, 0x02
	jnz .wait_input_status

	ret

.wait_output_status:
	cmp WORD [timer_counter], TIMEOUT_MS
	jae .keyboard_controller_failed

	in al, 0x64
	test al, 0x01
	jz .wait_input_status

	ret

.wait_a20_status:
	cmp WORD [timer_counter], TIMEOUT_MS
	mov ax, 1
	jae .wait_a20_status.exit

	call check_A20
	test ax, ax
	jnz .wait_a20_status

	xor ax, ax
.wait_a20_status.exit:
	ret

.exit:
	sti
	ret

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
 	mov [es:di], WORD 0x6969
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
	push ds

	; Set the interrupt handler for the PIT
	xor ax, ax
	mov ds, ax
	mov [PIT_ADDR + 2], cs
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
	
	pop ds
	sti

	ret

init_pit:
	cli
	mov al, 0x36
	out 0x43, al

	mov ax, 1194
	out 0x40, al
	mov al, ah
	out 0x40, al

	sti
	ret

timer_counter: dw 0
