;*************************************************
;	stdio.inc
;		-Input/Output routines
;
;	OS Development Series
;*************************************************
 
%ifndef __STDIO_INC_67343546FDCC56AAB872_INCLUDED__
%define __STDIO_INC_67343546FDCC56AAB872_INCLUDED__
 
;************************************************;
;	Puts16 ()
;		-Prints a null terminated string
;	DS=>SI: 0 terminated string
;************************************************;
 
bits	16
 
%macro printchar 0
        mov	ah, 0Eh	; Print AL
        mov	bx, 7	; 0000 0111b  bgcolor=black  forecolor(111)=white
        int	10h
%endm
 
Puts16:
		pusha				; save registers
.Loop1:
		lodsb				; load next byte from string from SI to AL
		cmp	al, 0			; Does AL=0?
		je	Puts16Done		; Yep, null terminator found-bail out
		mov	ah, 0eh			; Nope-Print the character
		mov	bx, 7
		int	10h			; invoke BIOS
		jmp	.Loop1			; Repeat until null terminator found
Puts16Done:
		popa				; restore registers
		ret				; we are done, so return

mitoa:
	and	al, 0x0F
	cmp	al, 9
	ja	.label10
	add	al, 0x30
	jmp	.labelshow
.label10:
	sub	al, 10
	add	al, 41h
.labelshow:
	ret

; cl: short int
; di: screen pos
; vPrinthex(short int, int screenpos)
vPrinthex: 
	push	es
	mov	ax, 0xB800
	mov	es, ax
	mov	al, cl
	shr	al, 4
	call mitoa
	stosb
	mov	al, 0x0F
	stosb
	mov	al, cl
	call mitoa
	stosb
	mov	al, 0x0F
	stosb
	pop es
	ret
; end hex

vPrinthexw: 
	mov  dl, cl
	mov  cl, ch
	call vPrinthex
	mov  cl, dl
	call vPrinthex
	ret

vPuts16:
	pusha
	mov	ax, 0xB800
	mov	es, ax
.Loop2:
	lodsb			; al=DS:SI
	cmp	al, 0
	je 	vPuts16Done
	stosb			; ES:DI=al
	mov	al, 0Fh
	stosb    
	jmp .Loop2

vPuts16Done:
	popa
	ret

; clear screen
clrscr:
	mov	cx, 80*24
	mov	ax, 0xB800
	mov	di, 0

	push	es
	mov	es, ax
loop1:
	mov	al, 0x20
	stosb
	mov	al, 0x0F
	stosb
	sub	cx, 1
	cmp	cx, 0
	jne 	loop1
	pop	es
	ret
; end clear screen


; domove(dx:count, data to es:di)
domove:
	mov	ax, 0x0
	push	es
	mov	es, ax
	cld
.Loop1:
%if 1
	lodsb
	stosb   ; copy ds:si to es:di
%else
	mov	ax, word [si]
	mov	word [di],ax
	add	di, 2
	add	si, 2
%endif

	;mov al, 0x2e
	;printchar

	sub dx, 1
	cmp dx, 0
	je .dumpdone
	jmp .Loop1
.dumpdone:
	pop es
	ret
 
%endif ;__STDIO_INC_67343546FDCC56AAB872_INCLUDED__
