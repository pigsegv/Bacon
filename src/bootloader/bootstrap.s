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
	mov dh, 0
	mov dl, 0
	int 10h

	mov ecx, msg
	mov ax, cx
	shr ecx, 16
	mov dx, cx
	mov cx, 0xffff
	div cx

	mov ds, ax
	mov si, dx

	mov ah, 0ah
	mov al, [msg]
	xor bh, bh
	mov cx, 5
	int 10h
	

.loop:
	
	

msg: db "Hello OS!", 10
len = $ - msg
	

