# qemu-system-ppc -S -nographic -gdb tcp::18181 -M ppce500 -kernel out/ppc/example/acamar-system/system
# powerpc-linux-gnu-gdb out/ppc/example/acamar-system/system -x packages/ppc/example/acamar-system.gdb
target remote :18181
# Don't prompt for terminal input
set height 0
b debug_println
b main
c
# The following is context switch A -> B -> A:
c
c
c
# The following is an arbitrarily chosen number of repetitions:
c
c
c
c
c
quit
y
