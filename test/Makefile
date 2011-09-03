SRCROOT=..
include $(SRCROOT)/mk.defines

ifeq ($(KNAME),$(KNAME_OSX))
MACHINE=pc
else
MACHINE=pc-0.11 
endif

TESTDIRS=$(wildcard test*)

# -redir tcp:4080::80
# tells qemu to listen on port 4080 and to forward connections to bos's port 80.
NET_OPT=-net user -net nic,model=i82559er -redir tcp:4080::80
all: run

run:
ifeq ($(KNAME),$(KNAME_CYGWIN))
	qemu -fda "$(IMG_NAME)" $(NET_OPT)
else
	qemu-system-x86_64 -boot a -M $(MACHINE) -fda "$(IMG_NAME)" $(NET_OPT)
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
