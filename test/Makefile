SRCROOT=..
include $(SRCROOT)/mk.defines

ifeq ($(KNAME),$(KNAME_OSX))
MACHINE=pc
else
MACHINE=pc-0.11 
endif

IMG_NAME = $(SRCROOT)/bos.img
TESTDIRS=$(wildcard *)

# -redir tcp:4080::80
# tells qemu to listen on port 6080 and to forward connections to bos's port 80.
NET_OPT=-netdev user,id=net0,hostfwd=tcp::6080-:80 -device i82559er,netdev=net0,mac=00:01:23:45:67:89 $(QEMUEXTRA)

ifneq ($(shell which qemu-system-x86_64),)
QEMU=qemu-system-x86_64
else
QEMU=qemu
endif
QEMUOSE=/usr/local/qemuose/bin/qemu
ifneq ($(shell which $(QEMUOSE)),)
ifeq ($(QDEBUG),1)
 QEMUEXTRA=-debug-e100
endif
QEMU=$(QEMUOSE) $(QEMUEXTRA)
else

# due to no qemu binary exists with qemu 2.1.2 on Mac OS X 10.10.5 brew install version, 
# try to use qemu-system instead
ifeq ($(shell which $(QEMU)),)
 QEMU=qemu-system-i386
 ifeq ($(shell which $(QEMU)),)
 $(error not found qemu cli, please install it before run)
 endif
endif
endif # for QEMUOSE custom install

all: run

run:
	@if [ "x$(IMG_NAME)" = "x" -o ! -f "$(IMG_NAME)" ]; then \
		echo "$(IMG_NAME) is not found, please make it in advance" ; \
		exit 1 ; \
	fi 
ifeq ($(KNAME),$(KNAME_CYGWIN))
	$(QEMU)$(QEMUMODEL) -fda "$(IMG_NAME)" $(NET_OPT)
else
	$(QEMU)$(QEMUMODEL) -M $(MACHINE) -drive format=raw,file=$(IMG_NAME),index=0,if=floppy $(NET_OPT)
endif

check:
	@echo OS=$(KNAME); echo VM=$(MACHINE)

clean:
	rm -f *.img
	@for i in $(TESTDIRS) ; do \
	if [ -d "$$i" ]; then \
		echo "clean $$i" ; \
		make -C $$i clean ; \
	fi done

pytest:
	make -C pytest

.PHONY: pytest
