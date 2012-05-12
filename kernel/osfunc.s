	
	extern _inthdr21, _inthdr27, _inthdr2c
	extern _inthdr20
	extern trap_handler

	global farjmp
	global load_tr, stor_tr, asm_getebp
	global asm_inthdr21, asm_inthdr27, asm_inthdr2c
	global asm_inthdr20

	; asm i/o port function for C
	global inb, outb
	global _inl, _outl

inb:
	mov  edx, [esp+4]
	mov  eax, 0
	in al, dx
	ret

outb:
	mov  edx, [esp+4]  ; io port
	mov  eax, [esp+8]  ; data
	out  dx, al
	ret

;uint32_t _intl(int port)
_inl:
	mov  edx, [esp+4]
	mov  eax, 0
	in eax, dx
	ret
;void _outl(int port, uint32_t data)
_outl:
	mov  edx, [esp+4]  ; io port
	mov  eax, [esp+8]  ; data
	out  dx, eax
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

	global _all_trap_handler

;; IDT_HANDLER_NOERR(name, trapno)
%macro IDT_HANDLER_NOERR 2
	global asm_%1
asm_%1:
	push 0 ; no error code
	push %2 ; trapno
	jmp _all_trap_handler
%endmacro
%macro IDT_HANDLER 2
	global asm_%1
asm_%1:
	push %2 ; trapno
	jmp _all_trap_handler
%endmacro

IDT_HANDLER_NOERR handler_0,0
IDT_HANDLER_NOERR handler_1,1
IDT_HANDLER_NOERR handler_2,2
IDT_HANDLER_NOERR handler_3,3
IDT_HANDLER_NOERR handler_4,4
IDT_HANDLER_NOERR handler_5,5
IDT_HANDLER_NOERR handler_6,6
IDT_HANDLER_NOERR handler_7,7
IDT_HANDLER handler_8,8
IDT_HANDLER_NOERR handler_9,9
IDT_HANDLER handler_10,10
IDT_HANDLER handler_11,11
IDT_HANDLER handler_12,12
IDT_HANDLER handler_13,13
IDT_HANDLER handler_14,14
IDT_HANDLER_NOERR handler_15,15
IDT_HANDLER_NOERR handler_16,16
IDT_HANDLER handler_17,17
IDT_HANDLER_NOERR handler_18,18
IDT_HANDLER_NOERR handler_19,19
IDT_HANDLER_NOERR handler_20,20

_all_trap_handler:
	;push es
	;push ds
	;pushal

	push esp
	call trap_handler
	add  esp, 4

	;popal
	;pop ds
	;pop es
	add  esp, 8
	iretd

	global trap_handlers
trap_handlers:
	dd asm_handler_0
	dd asm_handler_1
	dd asm_handler_2
	dd asm_handler_3
	dd asm_handler_4
	dd asm_handler_5
	dd asm_handler_6
	dd asm_handler_7
	dd asm_handler_8
	dd asm_handler_9
	dd asm_handler_10
	dd asm_handler_11
	dd asm_handler_12
	dd asm_handler_13
	dd asm_handler_14
	dd asm_handler_15
	dd asm_handler_16
	dd asm_handler_17
	dd asm_handler_18
	dd asm_handler_19
times 256-20 dd asm_handler_20
