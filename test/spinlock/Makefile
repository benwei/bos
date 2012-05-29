
LDFLAGS=-lpthread
BINS=spin_test spin_test_asm

all: $(BINS)

.c.o:
	$(CC) -c $<

.S.o:
	$(CC) -c $<

spin_test: spinlock.o main.o 
	$(CC) -o $@ $^ $(LDFLAGS)

spin_test_asm: spinlock_asm.o main.o 
	$(CC) -o $@ $^ $(LDFLAGS)

run: $(BINS)
	@for i in $^ ; do \
	echo "===== testing [$$i] =====" ; \
	./$$i; \
	done

clean:
	rm -f $(BINS) *.o
	# remove coredump on cygwin
	rm -f *.stackdump
