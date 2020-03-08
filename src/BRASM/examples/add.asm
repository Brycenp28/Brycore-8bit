main:

jmp %.start

.a:
	DZ 1
	DB 0
.b:
	DZ 1

.LC0:
	.str "Enter the first number (0-9): "
	DB 0
.LC1:
	DB 10
	.str "Enter the second number (0-9): "
	DB 0

.LC2:
	DB 10
	.str "first + second = "
	DB 0

.start:
	mov sp, 200
	mov a, %.LC0
	call %print
	mov a, %.a
	int 0
	mov a, %.LC1
	call %print
	mov a, %.b
	int 0
	mov a, %.LC2
	call %print
	mov a, [%.a]
	mov b, [%.b]
	sub a, 48
	sub b, 48
	add a, b
	add a, 48
	mov [%.a], a
	mov a, %.a
	call %print
	int 0
	hlt

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