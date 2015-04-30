/*| headers |*/
#include <stdint.h>
#include <stddef.h>

/*| object_like_macros |*/
/**
 * This implementation follows the conventions of the PowerPC EABI specification, obtained from:
 *
 * http://www.freescale.com/files/32bit/doc/app_note/PPCEABI.pdf
 *
 * The PowerPC EABI is also documented in detail in the following application note:
 *
 * https://www-01.ibm.com/chips/techlib/techlib.nsf/techdocs/852569B20050FF77852569970071B0D6/
 *
 * PowerPC EABI register usage:
 *  R0      Volatile: Language-specific
 *  R1      Dedicated: SP
 *  R2      Dedicated: Read-only small data area anchor
 *  R3-R4   Volatile: Parameter passing / return values
 *  R5-R10  Volatile: Parameter passing
 *  R11-R12 Volatile
 *  R13     Dedicated: Read-write small data area anchor
 *  R14-R31 Nonvolatile
 *  F0      Volatile: Language-specific
 *  F1      Volatile: Parameter passing / return values
 *  F2-F8   Volatile: Parameter passing
 *  F9-F13  Volatile
 *  F14-F31 Nonvolatile
 *  Fields CR2-CR4      Nonvolatile
 *  (Other CR fields)   Volatile
 *  (Other registers)   Volatile
 *
 * R0-R31 are the general-purpose registers (GPRs) and F0-F31 are the floating-point registers (FPRs).
 * CRn refers to the n'th of the eight 4-bit fields of the 32-bit condition register (CR).
 *
 * On context switch, we push non-volatile registers onto the stack following the EABI stack frame conventions:
 * The stack grows downwards, from high to low addresses, and is padded to ensure each stack frame is 8-byte aligned.
 * The creation of a stack frame takes place in the prologue of a function being called, and is optional in the sense
 * that leaf functions (that do not call any other functions) may opt not to do this.
 *
 * PowerPC EABI stack frame:
 *     +------------------------------------------------------------+ Highest address
 *     | FPR save area (optional)                                   |
 *     +------------------------------------------------------------+
 *     | GPR save area (optional)                                   |
 *     +------------------------------------------------------------+
 *     | CR save word (optional)                                    |
 *     +------------------------------------------------------------+
 *     | Local variables area (optional)                            |
 *     +------------------------------------------------------------+
 *     | Function parameters area (optional)                        |
 *     +------------------------------------------------------------+
 *     | Padding to adjust size to multiple of 8 bytes (1-7 bytes)  |
 *     +------------------------------------------------------------+
 *     | LR Save Word                                               |
 *     +------------------------------------------------------------+
 *     | Back Chain Word                                            |
 *     +------------------------------------------------------------+ Lowest address
 *
 * The bottom two words (LR Save & Back Chain) comprise the stack frame's header, and are the only compulsory fields.
 * The LR is the link register, used to track function return addresses.
 * The EABI states that the LR is to be saved in the LR save word of calling function's stack frame, not in the stack
 * frame of the currently executing function.
 * The back chain word points to the previous stack frame's back chain word field, forming a linked list.
 * The first (uppermost) stack frame shall have a back chain of 0 (NULL).
 */
#define CONTEXT_BC_IDX 0
#define CONTEXT_LR_IDX 1
#define CONTEXT_HEADER_SIZE (CONTEXT_LR_IDX + 1)
#define CONTEXT_GPR14_IDX 2
#define CONTEXT_GPR15_IDX 3
#define CONTEXT_GPR16_IDX 4
#define CONTEXT_GPR17_IDX 5
#define CONTEXT_GPR18_IDX 6
#define CONTEXT_GPR19_IDX 7
#define CONTEXT_GPR20_IDX 8
#define CONTEXT_GPR21_IDX 9
#define CONTEXT_GPR22_IDX 10
#define CONTEXT_GPR23_IDX 11
#define CONTEXT_GPR24_IDX 12
#define CONTEXT_GPR25_IDX 13
#define CONTEXT_GPR26_IDX 14
#define CONTEXT_GPR27_IDX 15
#define CONTEXT_GPR28_IDX 16
#define CONTEXT_GPR29_IDX 17
#define CONTEXT_GPR30_IDX 18
#define CONTEXT_GPR31_IDX 19
#define CONTEXT_FRAME_SIZE (CONTEXT_GPR31_IDX + 1)

/*| types |*/
typedef uint32_t* context_t;

/*| structures |*/

/*| extern_declarations |*/
extern void rtos_internal_context_switch(context_t *, context_t *) {{prefix_const}}REENTRANT;
extern void rtos_internal_context_switch_first(context_t *) {{prefix_const}}REENTRANT;

/*| function_declarations |*/
/**
 * Set up the initial execution context of a task.
 * This function is invoked exactly once for each task in the system.
  *
 * @param ctx An output parameter interpreted by the RTOS as the initial context for each task.
 *  After this function returns, the RTOS uses the value of ctx for task/context/stack switching.
 *  The concept of a context and of the context_t type is abstract and may have different implementations on
 *  different platforms.
 *  It can be, e.g., a stack pointer or a data structure for user-level task switching as on POSIX.
 *  This function is expected to set ctx to a value that the RTOS can pass to this platform's implementation of
 *  context_switch() and context_switch_first().
 *  The context must be set up such that the destination task of a task switch executes the code at the address fn
 *  using the memory region defined by stack_base and stack_size as its stack.
 *  For hardware platforms, this typically requires the following set up steps:
 *  - The value of ctx points to either the beginning or the end of the stack area.
 *  - The stack area contains fn so that the context-switch functions can pop it off the stack as a return address to
 *    begin execution at.
 *  - Optionally reserve additional stack space if the context-switch functions depend on it.
 * @param fn Points to a code address at which the given execution context shall start executing.
 *  This is typically a pointer to a parameter-less function that is assumed to never return.
 * @param stack_base Points to the lowest address of the memory area this execution context shall use as a stack.
 * @param stack_size The size in bytes of the stack memory area reserved for this execution context.
 */
static void context_init(context_t *ctx, void (*fn)(void), uint32_t *stack_base, size_t stack_size);

/*| state |*/

/*| function_like_macros |*/
#define context_switch(from, to) rtos_internal_context_switch(to, from)
#define context_switch_first(to) rtos_internal_context_switch_first(to)

/*| functions |*/
static void
context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size)
{
    uint32_t *const init_context = stack_base + stack_size - CONTEXT_HEADER_SIZE;
    uint32_t *const context = init_context - CONTEXT_FRAME_SIZE;

    /**
     * Set up an initial stack frame header containing just the back chain word and the LR save word.
     * The EABI specification requires that the back chain be NULL-terminated.
     * We set the LR save word to the task entry point.
     */
    init_context[CONTEXT_BC_IDX] = 0;
    init_context[CONTEXT_LR_IDX] = (uint32_t) fn;

    /**
     * Immediately below the dummy stack frame header, create a full-size stack frame containing register values for
     * the initial context.
     * The EABI spec requires that the back chain word always points to the previous frame's back chain word field.
     * LR save and back chain words in this frame are ignored by rtos_internal_context_switch (ctxt-switch.s)
     */
    context[CONTEXT_BC_IDX] = (uint32_t) &init_context[CONTEXT_BC_IDX];
    *ctx = context;
}

/*| public_functions |*/
