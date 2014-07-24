# qemu-system-ppc -S -nographic -gdb tcp::18181 -M ppce500 -kernel out/ppc/example/kraz-system/system
# powerpc-linux-gnu-gdb out/ppc/example/kraz-system/system -x packages/ppc/example/kraz-system.gdb
target remote :18181
# Don't prompt for terminal input
set height 0
b debug_println
b main
# This is an exact number of steps, after which a continue would loop forever - see kraz-test.c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c
quit
y
