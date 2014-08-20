# qemu-system-ppc -S -nographic -gdb tcp::18181 -M ppce500 -kernel <SYSTEM_BINARY>
# powerpc-linux-gnu-gdb <SYSTEM_BINARY> -x <THIS_FILE>
target remote :18181
# Don't prompt for terminal input
set height 0
b debug_println
b main
b tick_irq
b interrupt_event_wait
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
# First tick
echo Tick 1-1
c
c
c
c
# Second tick
echo Tick 1-2
c
c
c
c
# Third tick
echo Tick 1-3
c
c
c
c
# Fourth tick
echo Tick 1-4
c
c
c
c
# Fifth tick - b wakes up and does some things
echo Tick 1-5
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
# First tick
echo Tick 2-1
c
c
c
c
# Second tick
echo Tick 2-2
c
c
c
c
# Third tick
echo Tick 2-3
c
c
c
c
# Fourth tick
echo Tick 2-4
c
c
c
c
# Fifth tick - b wakes up and does some things
echo Tick 2-5
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
# Let it tick one more time, then finish up
echo Tick 3, Done.
quit
y
