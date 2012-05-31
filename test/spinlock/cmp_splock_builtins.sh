#!/bin/sh
gcc -Os -c spinlock_by_builtins.c
objdump.exe -d spinlock_by_builtins.o > spinlock_by_builtins_1.dasm
gcc -Os -c spinlock_by_builtins2.c
objdump.exe -d spinlock_by_builtins2.o > spinlock_by_builtins_2.dasm
diff spinlock_by_builtins_1.dasm spinlock_by_builtins_2.dasm
