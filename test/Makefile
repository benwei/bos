SRCROOT=..
include $(SRCROOT)/mk.defines

ifeq ($(KNAME),$(KNAME_OSX))
MACHINE=pc
else
MACHINE=pc-0.11 
endif

IMG_NAME = $(SRCROOT)/bos.img
TESTDIRS=$(wildcard test*)

# -redir tcp:4080::80
# tells qemu to listen on port 4080 and to forward connections to bos's port 80.
NET_OPT=-net user -net nic,model=i82559er -redir tcp:4080::80
QEMU=qemu
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
	$(QEMU) -fda "$(IMG_NAME)" $(NET_OPT)
else
	$(QEMU) -M $(MACHINE) -fda "$(IMG_NAME)" $(NET_OPT)
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
