use16
format binary

org 0x7c00

main:
	mov ah, 05h
	xor al, al
	int 10h

	mov ah, 00h
	mov al, 03h
	int 10h

	mov ah, 02h
	xor bh, bh
	mov dh, 12
	mov dl, 35
	int 10h

	mov ah, 0ah
	mov al, 'F'
	xor bh, bh
	mov cx, 5
	int 10h

	
	
msg: db "Hello OS!", 10
len = $ - msg
	

