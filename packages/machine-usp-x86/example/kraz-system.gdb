# qemu-system-ppc -S -nographic -gdb tcp::18181 -M ppce500 -kernel <SYSTEM_BINARY>
# gdb <SYSTEM_BINARY> -x <THIS_FILE>
# Don't prompt for terminal input
set height 0
b debug_println
b main
b complete
r
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
c
c
c
quit
y
