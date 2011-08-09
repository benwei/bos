	
	global farjmp
	global load_tr, stor_tr, asm_getebp

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

