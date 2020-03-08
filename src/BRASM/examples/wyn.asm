; whats your name program.
jmp %_start

LC0:
	.str "Whats your name?"
	DB 10 0

LC1:
	.str "Hello "
	DB 0

LC2:
	DB 46
	DB 0

; resb
name:
	DZ 16
	DB 0

_start:
	mov sp, 0
	mov sp, bp
	call %main
	hlt

main:
	mov a, %LC0
	call %print
	mov a, %name
	mov b, 16
	call %read

	mov a, %LC1
	call %print
	mov a, %name
	call %print
	mov a, %LC2
	call %print
	int 0
	ret

read: ; (char* buffer, char size) 30 bytes.
	push c
.read_loop:
	int 0
	inc a
	dec b
	mov c, [a-1]
	cmp c, 10
	jz %.read_exit
	cmp b, 0
	jnz %.read_loop
.read_exit:
	mov [a-1], 0
	xor a, a
	pop c
	ret


print: ; (char* location) 20 bytes.
	push b
.print_loop:
	int 1
	inc a
	mov b, [a]
	cmp b, 0
	jnz %.print_loop
	xor a, a
	pop b
	ret