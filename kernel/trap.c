#include "os_stdio.h"
#include "stdio.h"
#include "trap.h"

static void panic(const char *msg) {
	printf(msg);
	_syshalt();
}

static const char * const exception_desc[] = {
	"#DER: Divide error",
	"#DBG: Debug",
	"#NMI: Non-Maskable Interrupt",
	"#BRK: Breakpoint",
	"#OVF: Overflow",
	"#BRE: BOUND Range Exceeded",
	"#IOP: Invalid Opcode",
	"#DNA: Device Not Available",
	"#DOF: Double Fault",
	"#CSO: Coprocessor Segment Overrun",
	"#ITS: Invalid TSS",
	"#SNP: Segment Not Present",
	"#STF: Stack Fault",
	"#GEP: General Protection",
	"#PAF: Page Fault",
	"#UNT: (unknown trap)",
	"#XFP: x87 FPU Floating-Point Error",
	"#ALC: Alignment Check",
	"#MAC: Machine-Check",
	"#SFP: SIMD Floating-Point Exception"
};

static inline const char *
get_trap_desc(trapno)
{
	if (trapno < sizeof(exception_desc)/sizeof(exception_desc[0]))
		return exception_desc[trapno];
	return exception_desc[15];
}

/* 
 * using push esp to stack, trap_handler get trapframe struct
 */
void trap_handler(struct trapframe *tf)
{
	if (tf->cs == OS_KERN_CS) {
			printf("exception: %s\n eip:%08x,cs:%04x,eflags:%08x\n",
				get_trap_desc(tf->trapno),
				tf->eip, tf->cs, tf->eflags);
			/* encounter an issue that repeat trap message in kernel cs */
			if (tf->trapno == 0 || tf->trapno == 6) {
				panic("panic: system halt");
			}
			return;
	}

	printf("exception: %s\n eip:%08x,cs:%04x,eflags:%08x,esp:%08x,ss:%04x\n",
			get_trap_desc(tf->trapno),
			tf->eip, tf->cs, tf->eflags, tf->esp, tf->ss);
}
