CFILES= ../../blibc/itoa.c
OBJS=$(CFILES:.c=.o)
CFLAGS= -I ../../include
BLIBC_SHARED = libbosc.so


all: $(BLIBC_SHARED)
	python alltests.py

%.o :%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BLIBC_SHARED): $(OBJS)
	$(CC) -shared -o $@ $^

clean:
	rm -f $(BLIBC_SHARED) $(OBJS)
