/* GDT and IDT descriptor table */

#include "os_sys.h"
#include "trap.h"

#define SEG_NUM 1


void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) ADR_IDT;
	int i;

	/* GDT */
	for (i = 0; i <= LIMIT_GDT / 8; i++) {
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	/* code segment 1 */
	set_segmdesc(gdt + 1, LIMIT_SYSBOOT, ADR_SYSBOOT, AR_CODE32_ER);
	/* data segment 2 */
	set_segmdesc(gdt + 2, 0xffffffff, 0x00000000, AR_DATA32_RW);
	/* reference the begdt.inc */
	load_gdt(LIMIT_GDT, ADR_GDT);

	/* IDT 0x7FF*/
	for (i = 0; i <= LIMIT_IDT / 8; i++) {
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idt(LIMIT_IDT, ADR_IDT);

	/* setup interrupt handler */

	
	for (i = 0; i < 256; i++) {
		set_gatedesc(idt + i, (int) trap_handlers[i], OS_KERN_CS, AR_I386GATE32);
	}

	/* keyboard/irq1 SEN_NUM left shift 3. because the bit 0, 1, 2 should be zero */
	/* for timer interrupt */
	set_gatedesc(idt + 0x20, (int) asm_inthdr20, SEG_NUM * 8, AR_INTGATE32);
	/* for keyboard interrupt */
	set_gatedesc(idt + 0x21, (int) asm_inthdr21, SEG_NUM * 8, AR_INTGATE32);
	// set_gatedesc(idt + 0x27, (int) asm_inthdr27, SEG_NUM * 8, AR_INTGATE32);
	// set_gatedesc(idt + 0x2c, (int) asm_inthdr2c, SEG_NUM * 8, AR_INTGATE32);

	return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}
