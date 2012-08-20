#ifndef OS_SYS_H
#define OS_SYS_H

#include "types.h"

#ifndef BOS_VERSION
#define BOS_VERSION "0.2.4"
#endif

struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls;
	char leds; 
	char reserve;
	short scrnx, scrny; 
	char *vram;
};

#define ADR_BOOTINFO	0x00000ff0

/* asm global function */
void load_gdt(int limit, int addr);
void load_idt(int limit, int addr);

void asm_hlt(void);
void asm_stihlt(void);
void asm_cli(void);
void asm_sti(void);
void asm_inthdr20(void);
void asm_inthdr21(void);
void asm_inthdr27(void);
void asm_inthdr2c(void);


void farjmp(int eip, int cs);

/* for describtbl.c */
/* data segment */
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
/* Boot start address */
#define ADR_SYSBOOT		0x00000000
/* Limit is equal valid bytes - 1 = 512K - 1 */
#define LIMIT_SYSBOOT		0x0007ffff

#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_TSS32	0x0089
#define AR_INTGATE32	0x008e 
#define AR_I386GATE32	0x008e

#define TRAP_SYSCALL    68

/* for memory.c */
int io_load_eflags(void);
void io_store_eflags(int eflags);
int load_cr0(void);
int store_cr0(int cr0);
int load_tr(int tr);
int stor_tr(int tr);

unsigned int memtest_sub(unsigned int start, unsigned int end);

/* describtbl.c */
struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};
struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};

#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15


/* global descriptor tabale */
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);

/* interrupt.c */
void init_pic(void);

/* api will be called by asm interrupt handler */
void _inthdr21(int *esp);
void _inthdr27(int *esp);
void _inthdr2c(int *esp);
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#include "os_tss.h"
#include "libio.h"

#endif /* OS_SYS_H */
