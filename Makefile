SYSNAME=MYOS
SYSBIN=MYOS.BIN
SBINS=$(OUTPUT_DIR)/$(SYSBIN)
#IMG_NAME=/home/users/public/share/main.img
CURR_PWD=$(shell pwd)
IMG_NAME=$(CURR_PWD)/bos.img
OUTPUT_DIR=.
DD=dd

# source files

SHELL_SRC=kernel/init.c kernel/kthread.c $(shell ls lib/*.c) $(shell ls blibc/*.c) $(shell ls apps/*.c)
HW_DEP_ASM_SRC=kernel/main.s kernel/osfunc.s

# object files
KERN_OBJ = $(SHELL_SRC:.c=.o)
HW_DEP_ASM_OBJ = $(HW_DEP_ASM_SRC:.s=.o)

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
NASMW = nasm
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -I. -Iinclude -Iapps \
	-Wall -Werror -fno-builtin -O0 -g

LDSCRIPT := ld-script.lds

COMM_FLAGS=-nostdlib
COMM_LDFLAGS=--no-undefined
LDFLAGS = \
	$(COMM_FLAGS) -static -e _start \
	$(COMM_LDFLAGS) -X \
	-T $(LDSCRIPT) \
	-Map $(SYSNAME).map


COMM_OBJCOPYFLAGS=-R .pdr
OBJCOPYFLAGS = \
	-I elf32-i386 \
	-O binary $(COMM_OBJCOPYFLAGS) \
	-R .note -R .note.gnu.build-id -R .comment -S

BOOT_LOADER=bootldr.elf 

all: $(BOOT_LOADER) $(SYSBIN)

########################
# boot loader
########################
$(BOOT_LOADER): boot/bootldr.s 
	nasm -o $@ $< -I include

########################
# bootable floppy image
########################
$(IMG_NAME): $(BOOT_LOADER)
	$(DD) if=$(OUTPUT_DIR)/bootldr.elf of=$@ seek=0 count=1
	$(DD) if=/dev/zero of=$@ seek=1 count=2879

########################
# System binary
########################

# this needs the root permission to execute
package_by_mount: $(IMG_NAME) $(SYSBIN)
	ls -la $(SYSBIN)
	mkdir -p fda;\
	mount -o loop $(IMG_NAME) fda -o fat=12;\
	cp $(SYSBIN) fda;\
	umount fda;\
	rm -rf fda;

# I like this needs mcopy from mtools without root permission
package: $(IMG_NAME) $(SYSBIN)
	mcopy -i $(IMG_NAME) $(SYSBIN)  ::
	mdir -i $(IMG_NAME)

run: package
	make -C test IMG_NAME=$(IMG_NAME)

bin: $(SYSBIN)
hex:
	hexdump -C $(IMG_NAME)

.c.o:
	@if [ "$<" = "init.c" ] ; then \
		echo $< with -save-temps || exit 1 ; \
		$(CC) -c $(CFLAGS) $< -o $@ -save-temps || exit 1 ; \
	else \
		echo "building [$<] -> [$@]" ; \
		$(CC) -c $(CFLAGS) $< -o $@ || exit 1 ; \
	fi
	

# NASM + LD to ELF, objcopy to fat binary (Not working, it seems to be incompatible.)
MYOS.BIN: %.BIN: myos.elf ld-script.lds
	echo "convert $< $@"
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

myos.elf: $(KERN_OBJ) $(HW_DEP_ASM_OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

$(HW_DEP_ASM_OBJ) : %.o : %.s
	$(NASMW) -f elf32  $< -o $@  -Iinclude/ # -l $(@:.o=.lst)

clean:
	rm -f $(IMG_NAME) *.elf *.img $(SYSBIN) *.o *.lst *.map $(KERN_OBJ) $(HW_DEP_ASM_OBJ) 
