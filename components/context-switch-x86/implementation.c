/*| headers |*/
#include <stddef.h>
#include <stdint.h>

/*| object_like_macros |*/
#define CONTEXT_SIZE (sizeof(struct context))

/*| type_definitions |*/
typedef struct context* context_t;

/*| structure_definitions |*/
/* A C representation of the data stored on the stack by the x86 context-switch implementation.
 * Note that this data structure and the context-switch implementation need to be consistent. */
struct context
{
    uint32_t ebp_stack_frame;
    void (*return_address)(void);
};

/*| extern_definitions |*/
/* C declaration of the context switch implementation in assembly in packages/x86/ctxt-switch.s */
extern void rtos_internal_context_switch_x86(context_t *from, context_t to);

/*| function_definitions |*/

/*| state |*/

/*| function_like_macros |*/
/* context_switch(context_t *from, context_t *to) is a component API; translate it to the implementation; */
#define context_switch(from, to) rtos_internal_context_switch_x86(from, *to)
/* context_switch_first(context_t *to) is a component API; translate it to the implementation; */
#define context_switch_first(to)\
    do {\
        context_t unused_context;\
        context_switch(&unused_context, to);\
    } while(0);

/*| functions |*/
static void
context_init(context_t *const ctx, void (*const task_function)(void), uint8_t *const stack_base, const size_t stack_size)
{
    /* x86 uses a pre-decrement stack.
     * The initial stack pointer is set up to point to the high-address end of the task's stack area, with enough room
     * for the initial context that is initialized in this function.
     * The initial stack pointer is set so that, upon entering the task function, it is aligned at a 16-byte boundary.
     * This ensures compliance with x86 gcc calling conventions. */
    uint32_t stack_top_address = (uint32_t)stack_base + stack_size;
    *ctx = (context_t)(((stack_top_address - CONTEXT_SIZE) & 0xFFFFFFF0UL) + CONTEXT_SIZE);
    /* When the context-switch implementation switches the first time to a task, it will find the data on the stack
     * that is set up here.
     * First, it pops context->ebp_stack_frame from the stack into the ebp register.
     * Second, it pops context->return_address from the stack into the instruction pointer register, effectively
     * causing a jump.
     * The return address is set up to be the task function of the respective task.
     * As per the x86 calling convention, the first action of that function is to push the value of the ebp register
     * onto the stack and move the stack pointer (from the esp register) into the ebp register.
     * Therefore, the value of context->ebp_stack_frame is never evaluated and is therefore irrelevant. */
    (*ctx)->return_address = task_function;
}

/*| public_functions |*/
