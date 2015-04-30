/*| headers |*/
#include <ucontext.h>
#include <stdint.h>
#include <stddef.h>

/*| object_like_macros |*/

/*| types |*/
typedef ucontext_t context_t;

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
static void context_init(context_t *const ctx, void (*const fn)(void), uint8_t *const stack_base, const size_t stack_size);

/*| state |*/

/*| function_like_macros |*/
#define context_switch(from, to) swapcontext(from, to)
#define context_switch_first(to) setcontext(to)

/*| functions |*/
static void
context_init(context_t *const ctx, void (*const fn)(void), uint8_t *const stack_base, const size_t stack_size)
{
    getcontext(ctx);
    ctx->uc_stack.ss_sp = stack_base;
    ctx->uc_stack.ss_size = stack_size;
    ctx->uc_link = NULL;
    makecontext(ctx, fn, 0);
}

/*| public_functions |*/
