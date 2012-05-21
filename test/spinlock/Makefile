CFLAGS=-DMAX_LOCK_TIME=4
LDFLAGS=-lpthread
BINS=spin_test spin_test_asm spin_test_builtins spin_test_builtins2
PLATFORM=$(shell uname -s)
ifeq ($(PLATFORM),Linux)
CFLAGS+=-DHAVE_LINUX
endif

all: $(BINS)

.c.o:
	$(CC) -c $< $(CFLAGS)

.S.o:
	$(CC) -c $< $(CFLAGS)

spin_test: spinlock.o main.o 
	$(CC) -o $@ $^ $(LDFLAGS)

spin_test_asm: spinlock_asm.o main.o 
	$(CC) -o $@ $^ $(LDFLAGS)

spin_test_builtins: spinlock_by_builtins.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

spin_test_builtins2: spinlock_by_builtins2.o main.o
	$(CC) -o $@ $^ $(LDFLAGS)

run: $(BINS)
	@for i in $^ ; do \
	echo "===== testing [$$i] =====" ; \
	./$$i; \
	done

clean:
	rm -f $(BINS) *.o *.dasm
	# remove coredump on cygwin
	rm -f *.stackdump
