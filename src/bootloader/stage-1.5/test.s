format binary
use32
org 0x7e00 + 631

jmp main

main:
	mov al, 0xa
	mov dx, 0x3d4
	out dx, al
	mov al, 0x20
	mov dx, 0x3d5
	out dx, al

	mov al, 'H'
	mov ah, 0x2f
	mov [0xb8000], ax

	mov al, 'e'
	mov ah, 0x2f
	mov [0xb8002], ax

	mov al, 'l'
	mov ah, 0x2f
	mov [0xb8004], ax

	mov al, 'l'
	mov ah, 0x2f
	mov [0xb8006], ax

	mov al, 'o'
	mov ah, 0x2f
	mov [0xb8008], ax

	jmp exit

exit:
	jmp exit
	
crash:

test_write: rb 1

