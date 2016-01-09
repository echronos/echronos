/*| headers |*/
#include <stddef.h>
#include <stdint.h>

/*| object_like_macros |*/
#define CONTEXT_SIZE (sizeof(struct context))

/*| types |*/
typedef struct context* context_t;

/*| structures |*/
/**
 * A C representation of the data stored on the stack by the x86 context-switch implementation.
 * Note that this data structure and the context-switch implementation need to be consistent.
 * Also note that the RTOS does not store this data structure as the per-task context information.
 * Instead, it just stores a stack pointer for each inactive task.
 * This data structure describes the data that resides at such a stored stack pointer of an inactive task.
 *
 * The RTOS core uses the type `context_t` for storing the execution context of an inactive task.
 * The x86 context switch implementation stores all task execution context on the task's stack before a context
 * switch.
 * Therefore, the only task execution context the RTOS core needs to handle is a task's stack pointer.
 * `context_t` is therefore a pointer type and `struct context` describes the data that can be found at such a
 * pointer address.
 */
struct context
{
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp_stack_frame;
    void (*return_address)(void);
};

/*| extern_declarations |*/
/** C declaration of the context switch implementation in assembly in packages/x86/ctxt-switch.s */
extern void rtos_internal_context_switch_x86(context_t *from, context_t to);

/*| function_declarations |*/
static void context_init(context_t *ctx, void (*task_function)(void), uint8_t *stack_base, size_t stack_size);

/*| state |*/

/*| function_like_macros |*/
/* context_switch(context_t *from, context_t *to) is a component API; translate it to the implementation; */
#define context_switch(from, to)\
    do\
    {\
        if (from != to)\
        {\
            rtos_internal_context_switch_x86(from, *to);\
        }\
    } while(0);

/* context_switch_first(context_t *to) is a component API; translate it to the implementation; */
#define context_switch_first(to)\
    do\
    {\
        context_t unused_context;\
        context_switch(&unused_context, to);\
    } while(0);

/*| functions |*/
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
static void
context_init(context_t *const ctx, void (*const task_function)(void), uint8_t *const stack_base, const size_t stack_size)
{
    /* x86 uses a pre-decrement stack.
     * The initial stack pointer is set up to point to the high-address end of the task's stack area minus the size of
     * the initial context set up in this function.
     * The initial stack pointer is set so that, upon entering the task function, the stack pointer is aligned at a
     * 16-byte boundary.
     * This ensures compliance with x86 gcc calling conventions. */
    uint32_t stack_top_address = (uint32_t)stack_base + stack_size;
    *ctx = (context_t)((stack_top_address & 0xFFFFFFF0UL) - CONTEXT_SIZE);
    /* When the context-switch implementation switches the first time to a task, it will find the data on the stack
     * that is set up here.
     * First, it pops context->ebx,esi,edi, and ebp_stack_frame from the stack into the corresponding registers.
     * Second, it pops context->return_address from the stack into the instruction pointer register, effectively
     * causing a jump.
     * The return address is set up to be the task function of the respective task.
     * As per the x86 calling convention, the first action of that function is to push the value of the ebp register
     * onto the stack and move the stack pointer (from the esp register) into the ebp register.
     * Therefore, the value of context->ebp_stack_frame is never evaluated and is therefore irrelevant.
     * Also, the values for the ebx, esi, and edi fields in context are irrelevant for the first context switch. */
    (*ctx)->return_address = task_function;
}

/*| public_functions |*/
