SRCROOT=../..

CFILES=$(SRCROOT)/blibc/itoa.c testmain.c
TARGET_BIN=testblibc

all: test

$(TARGET_BIN): $(CFILES)
	gcc -o $(TARGET_BIN) $(CFILES)

test: $(TARGET_BIN)
	./$(TARGET_BIN)

clean:
	rm -f $(TARGET_BIN)
