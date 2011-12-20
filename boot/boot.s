.code16
.text

.set STACK_SEG , 0x07e0
.set STACK_OFF , 0x00ff
.set FAT_OFF   , 0x7f00
.set LOADER_SEG, 0x0050
.set LOADER_OFF, 0x0000

#############################################
# main procedure
#############################################
.global start
start:
	mov     %cs, %ax
	mov     %ax, %ds
	mov     %ax, %es

	mov     $STACK_SEG, %ax
	mov     %ax, %ss
	mov     $STACK_OFF, %sp

	movb    %dl, (BS_DrvNum)

	call    load_fat_and_root
	call    get_loader_cluster
	cmp     $0xffff, %ax
	jnz     launch_loader

	mov     $'@', %al
	call    putchar
	int     $0x18

#############################################
# load root directory
#############################################

load_fat_and_root:
	push    %ax
	push    %bx
	push    %cx
	push    %dx
	push    %si

	/********************************************
	 * Load FAT
	 ********************************************/
	mov     $msg_fat, %si
	call    print

	movw    (BPB_FATSz16), %cx
	movw    (BPB_RsvdSecCnt), %ax
	mov     $FAT_OFF, %bx
	call    read_sectors

	movw    (BPB_BytsPerSec), %ax
	mul     %cx
	add     %ax, %bx
	mov     %bx, (root_dir_base)
	call    print_dec

	/********************************************
	 * Load root directory
	 ********************************************/
	mov     $msg_root_dir, %si
	call    print

	#calculate the number of sectors containing root directory
	mov     $32, %ax
	mulw    (BPB_RootEntCnt)
	addw    (BPB_BytsPerSec), %ax
	dec     %ax
	xor     %dx, %dx
	divw    (BPB_BytsPerSec)
	mov     %ax, %cx                      # cx = ((32 * BPB_RootEntCnt) + (BPB_BytsPerSec - 1)) / BPB_BytsPerSec
	mov     %ax, (data_sector_start)

	#calculate first sector number of root directory
	xor     %ax, %ax
	movb    (BPB_NumFATs), %al
	mulw    (BPB_FATSz16)
	add     (BPB_RsvdSecCnt), %ax         # ax = (BPB_NumFATs * BPB_FATSz16 + BPB_RsvdSecCnt)
	add     %ax, (data_sector_start)

	#load root directory to the address specified by root_dir_base
	call    read_sectors

	#show the amount of data loaded
	movw    (BPB_BytsPerSec), %ax
	mul     %cx
	call    print_dec

	pop     %si
	pop     %dx
	pop     %cx
	pop     %bx
	pop     %ax
	ret

msg_fat:           .ascii "FAT:\0"
msg_root_dir:      .ascii "ROOT:\0"
msg_crlf:          .ascii "\r\n\0"
root_dir_base:     .short 0
data_sector_start: .short 0

#############################################
#
# Get cluster number of loader
#
# output:
#   %ax = cluster number
#
#############################################

get_loader_cluster:
	/*
	 * Stanard 8.3 format:
	 *   Offset  Len  Meaning
	 *        0   11  8.3 file name
	 *       11    1  File attributes: 0x01:Readonly, 0x02:Hidden, 0x04:Sys, 0x08:Vol, 0x10:Dir, 0X20:Archive
	 *       12    1  Reserved for WinNT
	 *       13    1  Creation time in tenths of a second
	 *       14    2  Creation time: 5 bit: Hour, 6 bit: Minute, 5 bit: Second
	 *       16    2  Creation date: 7 bit: Year, 4 bit: Month, 5 bit: Day
	 *       18    2  Last accessed date. (See also Creation date.)
	 *       20    2  High 16 bits of first cluster number. (Zero for FAT12/FAT16.)
	 *       22    2  Last modification time. (See also Creation time.) 
	 *       24    2  Last modification date. (See also Creation date.)
	 *       26    2  Low 16 bits of first cluster number.
	 *       28    4  Size of the file in bytes.  
	 *       32
	 *
	 * Long file names:
	 *   Offset  Len  Meaning
	 *        0    1  Order in the sequence of long file name entries.
	 *        1   10  The first 5 wide characters.
	 *       11    1  File Attributes: 0x0f for long file name entry
	 *       12    1  Long entry type. Zero for file name entry.
	 *       13    1  Checksum.
	 *       14   12  The next 6 wide characters.
	 *       26    2  Always zero.
	 *       28    4  The final 2 wide characters.
	 *       32
	 *
         *   The name entries are placed in reverse order before the short name entry.
	 *   The sequence of the last entry, which is first met, will be in the form of ('A'+ n).
	 *   The sequences of the other entries are simply in the format of (n).
	 *   There're 13 wide characters in total for each long file name entry.
	 */

	push    %bx
	push    %cx
	push    %si

	mov     $-1, %ax
	mov     (root_dir_base), %bx
	mov     (BPB_RootEntCnt), %cx

_scan_entry:
	cmpb    $0x00, (%bx)
	jz      _leave_scan_entry
	cmpb    $0x0f, 11 (%bx)		#check file attributes of long file name entry
	jz      _next_entry

_check_short_file_name:
	push    %cx

	mov     $11, %cx
	mov     %bx, %si
	mov     $str_kernel, %di
        repz    cmpsb
	pop     %cx
	jnz     _next_entry

	movw    26 (%bx), %ax
	jmp     _leave_scan_entry

_next_entry:
	add     $32, %bx
	loopnz  _scan_entry

_leave_scan_entry:
	pop     %si
	pop     %cx
	pop     %bx
	ret

str_kernel:    .ascii "LOADER  BIN\0"

#############################################
#
# Launch loader
#
# input:
#   %ax = cluster number
#
#############################################

launch_loader:
	/*
	 * Value of FAT entry
	 *
	 *   FAT12        FAT16          FAT32
	 *   0x000        0x0000         0x00000000             Free cluster.
	 *   0x001        0x0001         0x00000001             Reserved.
	 *   0x002-0xFEF  0x0002-0xFFEF  0x00000002-0x0FFFFFEF  Used cluster, pointing to next cluster.
	 *   0xFF0-0xFF5  0xFFF0-0xFFF5  0x0FFFFFF0-0x0FFFFFF5  Reserved.
	 *   0xFF6        0xFFF6         0x0FFFFFF6             Reserved.
	 *   0xFF7        0xFFF7         0x0FFFFFF7             Bad sector in cluster or reserved cluster.
	 *   0xFF8-0xFFF  0xFFF8-0xFFFF  0x0FFFFFF8-0x0FFFFFFF  Last cluster in file (EOC)
	 */

	mov     $LOADER_SEG, %bx
	mov     %bx, %es
	mov     $LOADER_OFF, %bx
	xor     %ch, %ch
	movb    (BPB_SecPerClus), %cl

_load_cluster:
	cmp     $0x0002, %ax
	jb      _finish
	cmp     $0x0fef, %ax
	ja      _finish

	push    %ax
	sub     $2, %ax
	mul     %cx

	add     (data_sector_start), %ax

	call    read_sectors
	mov     %cx, %ax
	mulw    (BPB_BytsPerSec)
	add     %ax, %bx
	pop     %dx                     # cluster number (%ax)

	mov     %dx, %si
	shl     $1, %si
	add     %dx, %si
	shr     $1, %si			# %si = Cluster_Number * 3 / 2
	and     $1, %dx			# %ax = Cluster_Number * 3 % 2 = Cluster_Number % 2

	add     $FAT_OFF, %si
	mov     (%si), %ax
	or      %dx, %dx
	jz      _even_offset
_odd_offset:
	shr     $4, %ax
	jmp     _load_cluster
_even_offset:
	and     $0x0fff, %ax
	jmp     _load_cluster
_finish:
	ljmp    $LOADER_SEG, $LOADER_OFF

