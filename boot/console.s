.code16
.text

#############################################
#
# print string
#
# input:
# 	%ds:%si = asciiz string
#
#############################################

.global print
print:
	push    %ax
	push    %si
1:
	lodsb
	orb     %al, %al
	jz      2f
	mov     $0x0e, %ah
	int     $0x10
	jmp     1b
2:
	pop     %si
	pop     %ax
	ret

#############################################
#
# print one character
#
# input:
#       %al = ascii
#
#############################################
.global putchar
putchar:
	push    %ax
	mov     $0x0e, %ah
	int     $0x10
	pop     %ax
	ret

.if 0
#############################################
#
# print integer in hexdecimal string
#
# input:
#       %ax = integer
#
#############################################

.global print_hex
print_hex:
	push    %ax
	push    %cx
	push    %si

	mov     $(str_hex_prefix), %si
	call    print

	mov     %ax, %si
	mov     $4, %cx
1:
	shl     $4, %si
	shr     $12, %ax
	call    to_hex_char
	call    putchar
	mov     %si, %ax
	loopnz  1b

	pop     %si
	pop     %cx
	pop     %ax
	ret

to_hex_char:
	add     $'0', %al
	cmp     $'9', %al
	jbe     1f
	add     $('a' - ('9' + 1)), %al
1:
	ret

str_hex_prefix: .ascii " 0x\0"
.endif

#############################################
#
# print integer in decimal string
#
# input:
#       %ax = integer
#
#############################################

.global print_dec
print_dec:
	push    %ax
	push    %bx
	push    %cx
	push    %dx

	mov     $4, %cx
	mov     $divisors_of_tens, %bx
1:
	xor     %dx, %dx
	divw    (%bx)

	or      %ax, %ax
	jz      2f
	add     $'0', %al
	call    putchar
2:
	mov     %dx, %ax
	add     $2, %bx
	loopnz  1b

	add     $'0', %al
	call    putchar

	mov     $'\r', %al
	call    putchar
	mov     $'\n', %al
	call    putchar

	pop     %dx
	pop     %cx
	pop     %bx
	pop     %ax
	ret

divisors_of_tens: .short 10000, 1000, 100, 10

#############################################
#
# show the rolling bar at cursor
#
#############################################

.if 0
.global step_it
step_it:
	push    %ax
	push    %bx
	push    %cx
	push    %si
	mov     $str_msg_step, %si
	mov     $str_cur_step, %bx
	movb    (%si), %al
	mov     $4, %cx
1:
	cmpb    %al, (%bx)
	jz      2f
	inc     %bx
	loopnz  1b
	jmp     3f
2:
	sub     $str_cur_step, %bx
	add     $str_next_step, %bx
	movb    (%bx), %al
	movb    %al, (%si)
	call    print
3:
	pop     %si
	pop     %cx
	pop     %bx
	pop     %ax
	ret

str_cur_step:   .ascii "\\|/-"
str_next_step:  .ascii "|/-\\"
str_msg_step:   .ascii "\\\b\0"
.endif
