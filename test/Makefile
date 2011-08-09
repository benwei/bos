SRCROOT=..
ifneq ($(shell which sw_vers),)
CURR_OS=$(shell sw_vers -productName)
else
CURR_OS=$(shell uname -s)
endif

ifeq ($(CURR_OS),Mac OS X)
MACHINE=pc
else
MACHINE=pc-0.11 
endif
all: run

run:
	qemu-system-x86_64 -boot a -M $(MACHINE) -fda $(IMG_NAME)

check:
	@echo OS=$(CURR_OS); echo VM=$(MACHINE)

clean:
	rm -f *.img

