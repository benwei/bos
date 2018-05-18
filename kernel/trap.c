#include "os_stdio.h"
#include "os_sys.h"
#include "stdio.h"
#include "trap.h"
#include "syscall.h"
#include "os_mtask.h"

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
	if (trapno == TRAP_SYSCALL) {
		return "System Call";
	}
	if (trapno < sizeof(exception_desc)/sizeof(exception_desc[0]))
		return exception_desc[trapno];
	return exception_desc[15];
}


/* 
 * using push esp to stack, trap_handler get trapframe struct
 */
void print_tf(struct trapframe *tf)
{
	if (tf->cs == OS_KERN_CS) {
			printf("trap(%d): %s\n eip:%08x,cs:%04x,eflags:%08x\n",
				tf->trapno,get_trap_desc(tf->trapno),
				tf->eip, tf->cs, tf->eflags);
			/* encounter an issue that repeat trap message in kernel cs */
			return;
	}

	printf("exception: %s\n eip:%08x,cs:%04x,eflags:%08x,esp:%08x,ss:%04x\n",
			get_trap_desc(tf->trapno),
			tf->eip, tf->cs, tf->eflags, tf->esp, tf->ss);
}

int trap_handler(struct trapframe *tf)
{
	switch(tf->trapno) {
	case TRAP_SYSCALL:
		print_tf(tf);

		return kern_syscall(tf->regs.eax, 
			     tf->regs.edx,
			     tf->regs.ecx);
    case TRAP_GEP:
        // 2018-05-18 FIXME: found #GEP issue on vmware player 12.5.9 by testing
		print_tf(tf);
        panic("panic: system halt, not fix yet\n");
    case TRAP_QEMU_MOUSE_CLICK: // not handle yet
        // [fixed] got 44 trap_handler while mouse click the window of qemu at the first time
        // after qemu-system-x86_64 started (tested in QEMU version 2.12.0, OSX 10.13.4)
        printf("event mouse grap qemu window trapno(%u) eax: %04x, edx: %04x, ecx: %04x\n",
                 tf->trapno,
                 tf->regs.eax,
                 tf->regs.edx,
                 tf->regs.ecx);
		return 0;
	default:
		print_tf(tf);
		panic("panic: system halt\n");
	}
	return 0;
}
