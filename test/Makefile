SRCROOT=..
include ../mk.defines

ifeq ($(KNAME),Mac OS X)
MACHINE=pc
else
MACHINE=pc-0.11 
endif
all: run

run:
	qemu-system-x86_64 -boot a -M $(MACHINE) -fda $(IMG_NAME)

check:
	@echo OS=$(KNAME); echo VM=$(MACHINE)

clean:
	rm -f *.img

