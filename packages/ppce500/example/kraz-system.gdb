# qemu-system-ppc -S -nographic -gdb tcp::18181 -M ppce500 -kernel out/ppce500/example/kraz-system/system
# powerpc-linux-gnu-gdb out/ppce500/example/kraz-system/system -x packages/ppce500/example/kraz-system.gdb
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
