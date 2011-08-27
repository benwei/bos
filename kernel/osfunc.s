	
	extern _inthdr21, _inthdr27, _inthdr2c
	extern _inthdr20
	global farjmp
	global load_tr, stor_tr, asm_getebp
	global asm_inthdr21, asm_inthdr27, asm_inthdr2c
	global asm_inthdr20

	; asm i/o port function for C
	global inb, outb

inb:
	mov  edx, [esp+4]
	mov  eax, 0
	;dw 0x00eb, 0x00eb
	; in al, cl ; invalid combination of opcode and operands
	; in al, dx ; correct
	in al, dx
	ret
outb:
	mov  edx, [esp+4]  ; io port
	mov  eax, [esp+8]  ; data
	;out  bl, al  ; invalid combination of opcode and operands 
	;out  dx, al  ; invalid combination of opcode and operands 
	;dw 0x00eb, 0x00eb
	out  dx, al
	ret

; void farjmp(int eip, int cs);
; use for task switch in c
farjmp: 
		; eip = esp + 4
		; cs = esp + 8
		jmp	far	[esp+4]			; eip, cs
		ret

load_tr: ; void load_tr(int tr);
		ltr		[esp+4]			; tr
		ret

store_tr: ; void store_tr(int tr);
		str		[esp+4]			; tr
		ret

asm_getebp: ; asm_getebp(void *esp);
		mov  [esp+4], ebp
		ret

asm_inthdr20:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthdr20

	pop	eax
	popad
	pop	ds
	pop	es
	iretd

asm_inthdr21:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax

	call	_inthdr21

	pop	eax
	popad
	pop	ds
	pop	es
	iretd

asm_inthdr27:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthdr27
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

asm_inthdr2c:
	push	es
	push	ds
	pushad
	mov	eax,esp
	push	eax
	mov	ax,ss
	mov	ds,ax
	mov	es,ax
	call	_inthdr2c
	pop	eax
	popad
	pop	ds
	pop	es
	iretd

