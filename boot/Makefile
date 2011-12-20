include ../mk.defines

TARGET         = boot.bin

SRCS           = bpb.s     \
                 boot.s    \
                 disk.s    \
                 console.s

OBJS           = $(SRCS:.s=.o)

CROSS_COMPILE ?=
AS             = $(CROSS_COMPILE)as
LD             = $(CROSS_COMPILE)ld
OBJCOPY        = $(CROSS_COMPILE)objcopy

all:	$(TARGET)

.SUFFIXES: .s
.s.o:
	$(AS) -g $^ -o $@

%.elf:
	$(LD) -g -o $@ -T $(@:.elf=.ld) $^

%.bin:
	$(OBJCOPY) -j .text -O binary $^ $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) *.bin *.elf

-include *.d

bpb.o: bpb.s
boot.o: boot.s
disk.o: disk.s
console.o: console.s
boot.elf: bpb.o boot.o disk.o console.o
boot.bin: boot.elf

