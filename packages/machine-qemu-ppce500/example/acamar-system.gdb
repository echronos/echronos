# qemu-system-ppc -S -nographic -gdb tcp::18181 -M ppce500 -kernel <SYSTEM_BINARY>
# powerpc-linux-gnu-gdb <SYSTEM_BINARY> -x <THIS_FILE>
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
