

;*************************************************
;	Gdt.inc
;		-GDT Routines
;
;	OS Development Series
;*************************************************

%ifndef __GDT_INC_67343546FDCC56AAB872_INCLUDED__
%define __GDT_INC_67343546FDCC56AAB872_INCLUDED__

bits	16

;*******************************************
; InstallGDT()
;	- Install our GDT
;*******************************************

InstallGDT:

	cli                  ; clear interrupts
	pusha                ; save registers
	lgdt 	[toc]        ; load GDT into GDTR
	sti	                 ; enable interrupts
	popa                 ; restore registers
	ret	                 ; All done!

;*******************************************
; Global Descriptor Table (GDT)
;*******************************************
gdt_data: 
	dd 0                ; null descriptor
	dd 0 

; gdt code:	            ; code descriptor
	dw 0FFFFh           ; limit low
	dw 0                ; base low
	db 0                ; base middle
	db 10011010b        ; access
	db 11001111b        ; granularity
	db 0                ; base high

; gdt data:	            ; data descriptor
	dw 0FFFFh           ; limit low 
	dw 0                ; base low
	db 0                ; base middle

; Bit 0 (Bit 40 in GDT): Access bit (Used with Virtual Memory). Because we don't use virtual memory (Yet, anyway), we will ignore it by 0
; Bit 1 (Bit 41 in GDT): is the r/w bit. Its set for code selector, so we can read and execute data in the segment
; Bit 2 (Bit 42 in GDT): is the "expansion direction" bit. We will look more at this later.
; Bit 3 (Bit 43 in GDT): tells the processor this is a code or data descriptor. (It is set, so we have a code descriptor)
; Bit 4 (Bit 44 in GDT): Represents this as a "system" or "code/data" descriptor. This is a code selector by 1.
; Bits 5-6 (Bits 45-46 in GDT): is the privilege level (i.e., Ring 0 or Ring 3). 
; Bit 7 (Bit 47 in GDT): Used to indicate the segment is in memory (Used with virtual memory). Set to zero for now, since we are not using virtual memory yet

%define GD_RING 0       
%define GD_ACCESS(gd_vm, gd_ring, gd_scd1, gd_sc2, gd_expd, gd_uvm) ((gd_vm << 7) | (gd_ring << 5) | (gd_scd1) << 4) | (gd_scd2 << 3)| (gd_expd << 3) | gd_uvm < 2 )
	db 10010010b        ; access
	db 11001111b        ; granularity
	db 0                ; base high
end_of_gdt:

toc: 
	dw end_of_gdt - gdt_data - 1 	; limit (Size of GDT)
	dd gdt_data 			; base of GDT

idt_48:
	dw 0     ; idt limit = 0
	dw 0,0   ; idt base = 0L

; give the descriptor offsets names

%define NULL_DESC 0
%define CODE_DESC 0x8
%define DATA_DESC 0x10

%endif ;__GDT_INC_67343546FDCC56AAB872_INCLUDED__

