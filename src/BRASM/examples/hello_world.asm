jmp %main

string:
	.str "Hello, World!"
	DB 10 0

main:
	mov a, %string
	call %print
	hlt

print: ; 20 bytes
	push b
print_loop:
	int 1
	inc a
	mov b, [a]
	cmp b, 0
	jnz %print_loop
	xor a, a
	pop b
	ret