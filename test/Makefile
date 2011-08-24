SRCROOT=..
include $(SRCROOT)/mk.defines

ifeq ($(KNAME),$(KNAME_OSX))
MACHINE=pc
else
MACHINE=pc-0.11 
endif
all: run

run:
ifeq ($(KNAME),$(KNAME_CYGWIN))
	qemu -fda "$(IMG_NAME)"
else
	qemu-system-x86_64 -boot a -M $(MACHINE) -fda "$(IMG_NAME)"
endif

check:
	@echo OS=$(KNAME); echo VM=$(MACHINE)

clean:
	rm -f *.img

