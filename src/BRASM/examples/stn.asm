; converts a string to an number.
; by 'Brycenp28'.

jmp %_start

numstr:
	.str "106" ; string: numstr.

_start: ; starts the program with the stack location.
	xor sp, sp
	mov bp, sp
	call %main
	dr
	hlt

main: ; main function.
	push bp
	mov bp, sp
	push %numstr
	push 3
	call %str_to_num
	mov sp, bp
	pop bp
	ret

str_to_num: ; converts string to num.
	push bp
	mov bp, sp
	sub sp, 3
	mov c, [bp+3]
	mov d, [bp+2]
	mov [bp-1], c
	mov [bp-2], d
	push b
	push c
	push d
	
	dec d
	add c, d
	inc d

	mov b, 1

stn_loop: ; string to number loop.
	mov a, [c]
	sub a, 48
	mul b
	
	push b
	mov b, [bp-3]
	add b, a
	mov [bp-3], b
	pop b

	push a
	mov a, b
	mul 10
	mov b, a
	pop a

	dec c
	dec d
	cmp d, 0
	jnz %stn_loop
	mov a, [bp-3]
	pop d
	pop c
	pop b
	mov sp, bp
	pop bp
	ret
