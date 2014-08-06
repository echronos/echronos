# qemu-system-ppc -S -nographic -gdb tcp::18181 -M ppce500 -kernel out/ppce500/example/hello/system
# powerpc-linux-gnu-gdb out/ppce500/example/hello/system -x packages/ppce500/example/hello.gdb
target remote :18181
# Don't prompt for terminal input
set height 0
b debug_println
b main
c
c
quit
y
