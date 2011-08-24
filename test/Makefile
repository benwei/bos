SRCROOT=..
include ../mk.defines

ifeq ($(KNAME),Mac OS X)
MACHINE=pc
else
MACHINE=pc-0.11 
endif
all: run

run:
ifeq ($(KNAME),CYGWIN)
	qemu -fda "$(IMG_NAME)"
else
	qemu-system-x86_64 -boot a -M $(MACHINE) -fda "$(IMG_NAME)"
endif

check:
	@echo OS=$(KNAME); echo VM=$(MACHINE)

clean:
	rm -f *.img

