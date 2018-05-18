#ifndef TRAP_H
#define TRAP_H
#include "types.h"

struct tregs {
	uint32_t ecx;
	uint32_t edx;
	uint32_t eax;
}__attribute__((packed));

struct trapframe {
	struct tregs regs;
	int trapno;
	int errcode;
	int eip;
	int cs;   /* use 16 bit*/
	int eflags;
	/* below only crossing rings, such user to kernel */
	int esp;
	int ss;   /* use 16 bit */
};

#define OS_KERN_CS 0x00000008
enum TRAP_T {
	TRAP_DER = 0, // 0  Divide error
	TRAP_DBG, // 1  Debug
	TRAP_NMI, // 2  Non-Maskable Interrupt
	TRAP_BRK, // 3  Breakpoint
	TRAP_OVF, // 4  Overflow
	TRAP_BRE, // 5  BOUND Range Exceeded
	TRAP_IOP, // 6  Invalid Opcode
	TRAP_DNA, // 7  Device Not Available
	TRAP_DOF, // 8  Double Fault
	TRAP_CSO, // 9  Coprocessor Segment Overrun
	TRAP_ITS, // 10 Invalid TSS
	TRAP_SNP, // 11 Segment Not Present
	TRAP_STF, // 12 Stack Fault
	TRAP_GEP, // 13 General Protection
	TRAP_PAF, // 14 Page Fault
	TRAP_UNT, // 15 (unknown trap)
	TRAP_XFP, // 16 x87 FPU Floating-Point Error
	TRAP_ALC, // 17 Alignment Check
	TRAP_MAC, // 18 Machine-Check
	TRAP_SFP, // 19 SIMD Floating-Point Exception
    TRAP_20,
    TRAP_21,
    TRAP_22,
    TRAP_23,
    TRAP_24,
    TRAP_25,
    TRAP_26,
    TRAP_27,
    TRAP_28,
    TRAP_29,
    TRAP_30,
    TRAP_31,
    TRAP_32,
    TRAP_33,
    TRAP_34,
    TRAP_35,
    TRAP_36,
    TRAP_37,
    TRAP_38,
    TRAP_39,
    TRAP_40,
    TRAP_41,
    TRAP_42,
    TRAP_43,
    TRAP_QEMU_MOUSE_CLICK,
    TRAP_45,
    TRAP_46,
    TRAP_47,
    TRAP_48,
    TRAP_49,
    TRAP_LAST
};

extern int trap_handlers[];

#endif
