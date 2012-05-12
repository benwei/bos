
;*******************************************************
;
;	Stage2.asm
;		Stage2 Bootloader
;
;	OS Development Series
;*******************************************************

bits	16

global _start
; Remember the memory map-- 0x500 through 0x7bff is unused above the BIOS data area.
; We are loaded at 0x500 (0x50:0)

jmp	_start

;*******************************************************
;	Preprocessor directives
;*******************************************************

%include "stdio.inc"			; basic i/o routines
%include "stdio32.inc"			; basic i/o routines
%include "begdt.inc"			; Gdt routines
%include "A20.inc"			; A20 enabling

;*******************************************************
;	Data Section
;*******************************************************

LoadingMsg db 0x0D, 0x0A, "Searching for Operating System...", 0x00

;*******************************************************
;	STAGE 2 ENTRY POINT
;
;		-Store BIOS information
;		-Load Kernel
;		-Install GDT; go into protected mode (pmode)
;		-Jump to Stage 3
;*******************************************************

_start:

	;-------------------------------;
	;   Setup segments and stack	;
	;-------------------------------;

	cli				; clear interrupts
	xor	ax, ax			; null segments
	mov	ds, ax
	mov	es, ax
	mov	ax, 0x9000		; stack begins at 0x9000-0xffff
	mov	ss, ax
	mov	sp, 0xFFFF
	sti				; enable interrupts

	;-------------------------------;
	;   Install our GDT		;
	;-------------------------------;

	call	InstallGDT		; install our GDT

	;-------------------------------;
	;   Enable A20			;
	;-------------------------------;

	call	EnableA20_KKbrd_Out

	;-------------------------------;
	;   Print loading message	;
	;-------------------------------;

	mov	si, LoadingMsg
	call	Puts16

	;-------------------------------;
	;   Go into pmode		;
	;-------------------------------;

EnterStage3:

; load e820 info
	mov ebx, 0
	mov di, e820_data
.loop:
	mov eax, 0xE820
	mov ecx, 20
	mov edx,0x0534D4150 ; SMAP
	int 15h
	jc e820_chk_failed
	add di, 20
	inc DWORD [e820_num]
	cmp ebx, 0
	jne .loop
	jmp e820_chk_ok
e820_chk_failed:
	mov DWORD [e820_num], 0
e820_chk_ok:

	cli				; clear interrupts
	mov	eax, cr0		; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax

	jmp	CODE_DESC:Stage3	; far jump to fix CS

	; Note: Do NOT re-enable interrupts! Doing so will triple fault!
	; We will fix this in Stage 3.

;******************************************************
;	ENTRY POINT FOR STAGE 3
;******************************************************

bits 32
	; interrupt
	global load_gdt, load_idt

	global asm_stihlt
	global asm_hlt, asm_cli, asm_sti
	; memory handling
	global io_load_eflags, io_store_eflags, load_cr0, store_cr0
	global memtest_sub

	; asm system function for c
	global _syshalt, _nop

	; asm std out function for c
	global _cputs, _cputchar, _movcursor
	global _clrscr
	; asm for memory info
	global get_e820_data, get_e820_num

	; asm for gcc stack chk
	global  __stack_chk_fail 
	extern _benmain
Stage3:
	mov	ax, DATA_DESC		; set data segments to data selector (0x10)
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 90000h		; stack begins from 90000h

	call	_benmain
	add	esp,4
	;---------------------------------------;
	;   Stop execution			;
	;---------------------------------------;

_syshalt:
	cli
	hlt

asm_hlt:
	hlt
	ret


asm_cli:
	cli
	ret

asm_sti:
	sti
	ret

asm_stihlt:
	sti
	hlt
	ret

_nop:
	nop
	ret

_clrscr:
	call ClrScr32
	ret

_movcursor: ; _movcursor(char x, char y);
	mov eax, [esp+4] ; x
	mov ebx, [esp+8] ; y
	mov ah, bl
	call GotoXY
	ret

_cputs: ; void _cputs(char c);  bl = c;
	mov  ebx, [esp+4]
	call Puts32
	ret

_cputchar: ; void _cputs(char *s);  ebx = addrof(s);
	mov  ebx, [esp+4]
	call Putch32
	ret

__stack_chk_fail:
	mov ebx, msg_stack_chk_fail
	call Puts32
	ret

load_idt:
	mov ax, [ESP+4]
	mov [ESP+6], AX
	lidt [ESP+6]
	ret

load_gdt:
	mov ax, [ESP+4]
	mov [ESP+6], AX
	lgdt [ESP+6]
	ret

io_load_eflags:	; int io_load_eflags(void);
	pushfd		; push eflags 
	pop		eax
	ret

io_store_eflags:	; void io_store_eflags(int eflags);
	mov		eax,[esp+4]
	push	eax
	popfd		; pop eflags 
	ret

load_cr0:		; int load_cr0(void);
	mov		eax,cr0
	ret

store_cr0:		; void store_cr0(int cr0);
	mov		eax,[esp+4]
	mov		cr0,eax
	ret

memtest_sub:	; unsigned int memtest_sub(unsigned int start, unsigned int end)
		push	edi
		push	esi
		push	ebx
		mov	esi,0xaa55aa55		; pat0 = 0xaa55aa55;
		mov	edi,0x55aa55aa		; pat1 = 0x55aa55aa;
		mov	eax,[esp+12+4]		; i = start;
mts_loop:
		mov	ebx,eax
		add	ebx,0xffc		; p = i + 0xffc;
		mov	edx,[ebx]		; old = *p;
		mov	[ebx],esi		; *p = pat0;
		xor	dword [ebx],0xffffffff	; *p ^= 0xffffffff;
		cmp	edi,[ebx]		; if (*p != pat1) goto fin;
		jne	mts_fin
		xor	dword [ebx],0xffffffff	; *p ^= 0xffffffff;
		cmp	esi,[ebx]			; if (*p != pat0) goto fin;
		jne	mts_fin
		mov	[ebx],edx			; *p = old;
		add	eax,0x4000			; i += 0x4000;
		cmp	eax,[esp+12+8]			; if (i <= end) goto mts_loop;
		jbe	mts_loop
		pop	ebx
		pop	esi
		pop	edi
		ret
mts_fin:
		mov	[ebx],edx			; *p = old;
		pop	ebx
		pop	esi
		pop	edi
		ret

get_e820_num:	; int get_e820_num(void);
	mov eax, DWORD [e820_num]
	ret
get_e820_data:	; const unsigned char *get_e820_data(void);
	mov eax, e820_data
	ret

msg_stack_chk_fail db  "__stack_chk_fail", 0xa, 0
e820_num  dd  0
e820_data times 256 db 0
