/*| provides |*/
context-switch-ppce500
/*| requires |*/
/*| doc_header |*/
/*| doc_concepts |*/
/*| doc_api |*/
/*| doc_configuration |*/
/*| doc_footer |*/
# Toolchain Requirements

The context switch implementation for PowerPC e500 follows the conventions of the PowerPC EABI specification, obtained from <http://www.freescale.com/files/32bit/doc/app_note/PPCEABI.pdf>.
Thus, the user must configure their toolchain to compile the RTOS and user code to conform with EABI (for example, GCC provides the `-meabi` option for this purpose).

Because the EABI calling convention has the calling function assume responsibility for preserving the contents of any volatile registers it needs across a function call, this implementation preserves only those general-purpose registers which are non-volatile (i.e. `GPR14-GPR31`).

This context switch implementation does *not* preserve the small data anchor registers (i.e. `GPR2` and `GPR13`).
Thus, the user must judge whether it is appropriate to configure their toolchain to avoid use of the small data area optimization (for example, GCC provides the options `-mno-sdata` and `-G 0` to explicitly disable this feature).
