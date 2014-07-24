# qemu-system-ppc -S -nographic -gdb tcp::18181 -M ppce500 -kernel out/ppc/example/gatria-system/system
# powerpc-linux-gnu-gdb out/ppc/example/gatria-system/system -x packages/ppc/example/gatria-system.gdb
target remote :18181
# Don't prompt for terminal input
set height 0
b debug_println
b main
# A once-only starting sequence:
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
# First iteration of periodic sequence:
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
# The following is an arbitrarily chosen number of repetitions:
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
