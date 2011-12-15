/*****************************************************************************
 * Reference of FAT12/FAT16/FAT32 formats:
 * 	http://www.microsoft.com/whdc/system/platform/firmware/fatgen.mspx
 *****************************************************************************/
.code16
.text
.extern start
.global BPB_BytsPerSec
.global BPB_SecPerClus
.global BPB_RsvdSecCnt
.global BPB_NumFATs
.global BPB_RootEntCnt
.global BPB_FATSz16
.global BPB_SecPerTrk
.global BPB_NumHeads
.global BS_DrvNum

                /***************************************
                 * Common part for FAT12/FAT16/FAT32
                 ***************************************/
                jmp start                     # e9 <offset_16>
                #nop                          # eb <offset_8> 90

                .ascii "MSWIN4.1"             #+03 BS_OEMName:     OEM name (use MSWIN4.1 for compatibility)
BPB_BytsPerSec: .short 512                    #+11 BPB_BytsPerSec: Bytes per sector (possible values are 512, 1024, 2048, and 4096)
BPB_SecPerClus: .byte  1                      #+13 BPB_SecPerClus: Sectors per cluster (n^2: 1, 2, 4, 8, 16, 32, 64, and 128)
BPB_RsvdSecCnt: .short 1                      #+14 BPB_RsvdSecCnt: Reserved sector count (1 for FAT12/FAT16, 32 for FAT32)
BPB_NumFATs:    .byte  2                      #+16 BPB_NumFATs:    Number of FATs
BPB_RootEntCnt: .short 224                    #+17 BPB_RootEntCnt: Root entry count (512 for FAT16, 0 for FAT32)
                .short 2880                   #+19 BPB_TotSec16:   Total sector (16-bit). For FAT12/FAT16, BPB_TotSec16 != 0 && BPB_TotSec32 == 0. For FAT32, BPB_TotSec16 == 0 && BPB_TotSec32 != 0
                .byte  0xf0                   #+21 BPB_Media:      0xf0 for removal media, 0xf8 for fixed media (available values: 0xf0 - 0xff)
BPB_FATSz16:    .short 9                      #+22 BPB_FATSz16:    Sectors per FAT (16-bit) for FAT12/FAT16. 0 for FAT32.
BPB_SecPerTrk:  .short 18                     #+24 BPB_SecPerTrk:  Sectors per track
BPB_NumHeads:   .short 2                      #+26 BPB_NumHeads:   Number of heads (2 for 1.44 MB 3.5-inch floppy)
                .int   0                      #+28 BPB_HiddSec:    Hidden sectors (0 for non-partitioned media)
                .int   0                      #+32 BPB_TotSec32:   Total sector (32-bit) (BPB_TotSec32 >= 0x10000 when BPB_TotSec16 == 0)

.ifndef FAT32
                /***************************************
                 * FAT12/FAT16 specific fields
                 ***************************************/
BS_DrvNum:      .byte  0                      #+36 BS_DrvNum:      Drive number (0x00 for FDD, 0x80 for HDD)
                .byte  0                      #+37 BS_Reserved1:   Reserved (used by WindowsNT) (= 0)
                .byte  0x29                   #+38 BS_BootSig:     Boot signature (= 0x29) indicating the following 3 fields are present.
                .int   0                      #+39 BS_VolID:       Volume serial number. (It is usually assigned with timestamp.)
                .ascii "BOS BootDsk"          #+43 BS_VolLab:      Volume label (11 bytes = 8 + 3). It's likely to use "NO NAME    " by default.
                .ascii "FAT12   "             #+54 BS_FileSysType: File system type: "FAT12   ", "FAT16   ", or "FAT     "
                                              #+62
.else
                /***************************************
                 * FAT32 specific fields
                 ***************************************/
BPB_FATSz32:    .int   0                      #+36 BPB_FATSz32:    Sectors per FAT for FAT32. (BPB_FATSz16 should be 0.)
                .short 0                      #+40 BPB_ExtFlags:   Extended flags for FAT32.
                .short 0                      #+42 BPB_FSVer:      Hi-byte: Major version number, lo-byte: Minor version number. 0:0 for ignore.
BPB_RootClus:   .int   2                      #+44 BPB_RootClus:   Cluster number of the first cluster of the root directory
                .short 1                      #+48 BPB_FSInfo:     Sector number of FSINFO structure
                .short 6                      #+50 BPB_BkBootSec:  Sector number of backup boot record
                .space 12                     #+52 BPB_Reserved:   Reserved
BS_DrvNum:      .byte  0                      #+64 BS_DrvNum:      Drive number (0x00 for FDD, 0x80 for HDD)
                .byte  0                      #+65 BS_Reserved1:   Reserved
                .byte  0x29                   #+66 BS_BootSig:     Boot signature (= 0x29) indicating the following 3 fields are present.
                .int   0                      #+67 BS_VolID:       Volume serial number. (It is usually assigned with timestamp.)
                .ascii "BOS BootDsk"          #+71 BS_VolLab:      Volume label (11 bytes = 8 + 3). It's likely to use "NO NAME    " by default.
                .ascii "FAT12   "             #+82 BS_FileSysType: File system type: "FAT12   ", "FAT16   ", or "FAT     "
                                              #+90
.endif

		/***************************************
		* signature at the end of boot sector
		***************************************/
.section .signature
                .byte 0x55
                .byte 0xaa

