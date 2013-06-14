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
# tells qemu to listen on port 4080 and to forward connections to bos's port 80.
NET_OPT=-net user -net nic,model=i82559er -redir tcp:4080::80 $(QEMUEXTRA)
#NET_OPT=-net user -net nic,model=i82559er $(QEMUEXTRA)
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
endif

all: run

run:
	@if [ "x$(IMG_NAME)" = "x" -o ! -f "$(IMG_NAME)" ]; then \
		echo "$(IMG_NAME) is not found, please make it in advance" ; \
		exit 1 ; \
	fi 
ifeq ($(KNAME),$(KNAME_CYGWIN))
	$(QEMU)$(QEMUMODEL) -fda "$(IMG_NAME)" $(NET_OPT)
else
	$(QEMU)$(QEMUMODEL) -M $(MACHINE) -fda "$(IMG_NAME)" $(NET_OPT)
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
