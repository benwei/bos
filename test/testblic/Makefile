SRCROOT=../..
include $(SRCROOT)/mk.defines

CFLAGS=-I $(SRCROOT)/include
CFILES=$(SRCROOT)/blibc/itoa.c testmain.c
TARGET_BIN=testblibc$(EXEEXT)

all: test

$(TARGET_BIN): $(CFILES)
	gcc -o $(TARGET_BIN) $(CFILES) $(CFLAGS)

test: $(TARGET_BIN)
	./$(TARGET_BIN)

clean: $(TARGET_BIN)
	rm -f $(TARGET_BIN)
