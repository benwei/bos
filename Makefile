SYSNAME=MYOS
SYSBIN=MYOS.BIN
SBINS=$(OUTPUT_DIR)/$(SYSBIN)
IMG_NAME=bos.img
OUTPUT_DIR=.

# defines for global use
include ./mk.defines

ARCH=i386
include mk.$(ARCH)
KERN_INCS = $(ARCH_INCS)
KERN_INCS +=-Iinclude

# source files
USER_SRC= $(shell ls user/*.c) 
FS_SRC= $(shell ls fs/*.c) $(shell ls fs/vfs/*.c) $(shell ls fs/ramfs/*.c)
KERN_SRC = $(wildcard kernel/*.c) 
SHELL_SRC= $(shell ls lib/*.c) $(shell ls lib/errno/*.c) $(shell ls blibc/*.c) $(shell ls apps/*.c) $(FS_SRC)
HW_DEP_ASM_SRC=kernel/main.s kernel/osfunc.s 

# object files
KERN_OBJS += $(KERN_SRC:.c=.o)
KERN_OBJS += $(ARCH_OBJS)
KERN_OBJS += $(SHELL_SRC:.c=.o)
KERN_OBJS += $(USER_SRC:.c=.o)

HW_DEP_ASM_OBJ = $(HW_DEP_ASM_SRC:.s=.o)

NASMW = nasm
LDSCRIPT := ld-script.lds



ifeq ($(KNAME),$(KNAME_OSX))
NASMW_LDFLAGS = -f elf
EXTRA_CFLAGS=-m32
else
EXTRA_CFLAGS=-m32
NASMW_LDFLAGS = -f elf32
endif

COMM_FLAGS=-nostdlib
COMM_LDFLAGS=--no-undefined -T $(LDSCRIPT) -Map $(SYSNAME).map 
CFLAGS =$(EXTRA_CFLAGS) -I. $(KERN_INCS) -Iapps \
	-Wall -fno-builtin -O0 -g

ifeq ($(KNAME),$(KNAME_CYGWIN))
# add some code for cygwin environment
textAddr=1000
LDFLAGS = $(COMM_FLAGS) -static -e _start -s -Ttext $(textAddr) -Map $(SYSNAME).map 
else

LDFLAGS = \
	$(COMM_FLAGS) -static -e _start \
	$(COMM_LDFLAGS) -X 
endif


COMM_OBJCOPYFLAGS=-R .pdr
OBJCOPYFLAGS = \
	-I elf32-i386 \
	-O binary $(COMM_OBJCOPYFLAGS) \
	-R .note -R .note.gnu.build-id -R .comment -S

BOOT_LOADER=boot/boot.bin 
OS_LOADER=LOADER.BIN

all: $(BOOT_LOADER) $(SYSBIN)

########################
# boot loader
########################
$(BOOT_LOADER):
	make -C boot

$(OS_LOADER): loader/loader.s 
	nasm -o $@ $< -I include

########################
# bootable floppy image
########################
$(IMG_NAME): $(BOOT_LOADER)
	$(DD) if=$(BOOT_LOADER) of=$@ seek=0 count=1
	$(DD) if=/dev/zero of="$@" skip=1 seek=1 bs=512 count=$$((2880 - 1))

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

# I like this because use mcopy from mtools without root permission for packaging
package: $(OS_LOADER) $(IMG_NAME) $(SYSBIN)
	mcopy -n -o -i "$(IMG_NAME)" "$(OS_LOADER)" ::
	mcopy -n -o -i "$(IMG_NAME)" "$(SYSBIN)"  ::
	mdir -i "$(IMG_NAME)"

run: package
	@#only support in well-setup Ubuntu and OSX(only tried 10.6.8)
	make -C test IMG_NAME="../$(IMG_NAME)" QEMUEXTRA=$(QEMUEXTRA) \
		QEMUMODEL=$(QEMUMODEL)

update: $(SYSBIN)
	mcopy -n -o -i "$(IMG_NAME)" "$(SYSBIN)"  ::
	mdir -i "$(IMG_NAME)"

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
	@echo "convert $< to $@"
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

myos.elf: $(HW_DEP_ASM_OBJ) $(KERN_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(HW_DEP_ASM_OBJ) : %.o : %.s
	$(NASMW) $(NASMW_LDFLAGS)  $< -o $@  -Iinclude/ 

info:
	@echo "PLATFORM=[$(KNAME)]"

clean:
	make -C boot clean
	rm -f $(IMG_NAME) *.elf *.img $(SYSBIN) *.o *.lst *.map $(KERN_OBJS) $(HW_DEP_ASM_OBJ) $(OS_LOADER)
	make -C test clean

ctags:
	ctags -R .
