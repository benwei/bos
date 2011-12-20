.code16
.text

#############################################
#
# read sectors
#
# input:
#    %ax     = sector number
#    %cx     = count
#    %es:%bx = buffer
#
# output:
#    %ax     = result code
#    %cx     = count
#
#############################################

.global read_sectors
read_sectors:
	/******************************************************
	 * LBA to CHS
	 *     Sector = (LBA % BPB_SecPerTrk) + 1
	 *       Head = (LBA / BPB_SecPerTrk) % BPB_NumHeads
	 *   Cylinder = (LBA / BPB_SecPerTrk) / BPB_NumHeads
	 ******************************************************/
	push    %dx
	push    %si
	push    %cx

	xor     %dx, %dx
	divw    (BPB_SecPerTrk)
	inc     %dl                  
	mov     %dl, %cl             # cl = sector

	xor     %dx, %dx
	divw    (BPB_NumHeads)
	mov     %al, %ch             # ch = cylinder(track)
	mov     %dl, %dh             # dh = head

	movb    (BS_DrvNum), %dl     # dl = drive

	pop     %ax
	mov     $0x02, %ah           #INPUT  al: number of sectors, ch: track, cl: sector, dh: head, dl: drive, es:bx: buffer
	int     $0x13                #OUTPUT ah: return code, al: number of sectors read
	mov     %ax, %cx
	jc      1f

	#reset drive
	xor     %ah, %ah
	int     $0x13
	jmp     2f
1:
	#let it fail immediately
	mov     $'!', %ax
	call    putchar
	int     $0x18
2:
	mov     %ch, %al
	xor     %ch, %ch
	pop     %si
	pop     %dx
	ret

