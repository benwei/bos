SRCROOT=../..
include $(SRCROOT)/mk.defines

CFLAGS=-I $(SRCROOT)/include -I.
CFILES=$(SRCROOT)/blibc/itoa.c $(wildcard *.c)
OBJS=$(CFILES:.c=.o)
TARGET_BIN=testblibc$(EXEEXT)

all: test

.c.o:
	gcc -c $< -o $@ $(CFLAGS)

$(TARGET_BIN): $(OBJS)
	gcc -o $(TARGET_BIN) $^ $(LDFLAGS)

test: $(TARGET_BIN)
	./$(TARGET_BIN)

clean: 
	rm -f $(TARGET_BIN) $(OBJS)
