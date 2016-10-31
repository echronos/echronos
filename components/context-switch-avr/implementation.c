/*| headers |*/
#include <stddef.h>
#include <stdint.h>

/*| object_like_macros |*/
#define STACK_TASK_FUNCTION_ADDRESS_LENGTH sizeof(void*)
#define STACK_TASK_REGISTER_STATE_LENGTH 20U
#define STACK_POP_PRE_INCREMENT_ADJUSTMENT 1U

/*| types |*/
typedef void * context_t;

/*| structures |*/

/*| extern_declarations |*/
extern void context_switch_avr(context_t to, context_t *from);
extern void context_switch_first_avr(context_t to);

/*| function_declarations |*/
static void context_init(context_t *const ctx, void (*const fn)(void), uint8_t *const stack_base, const size_t stack_size);

/*| state |*/

/*| function_like_macros |*/
#define context_switch(from, to) do { if (from != to) context_switch_avr(*to, from); } while(0)
#define context_switch_first(to) context_switch_first_avr(*to);

/*| functions |*/
static void
context_init(context_t *const ctx, void (*const fn)(void), uint8_t *const stack_base, const size_t stack_size)
{
    stack_base[stack_size - 1] = (uint16_t)fn & 0xFF;
    stack_base[stack_size - 2] = (uint16_t)fn >> 8;
    *ctx = &stack_base[stack_size - (STACK_TASK_FUNCTION_ADDRESS_LENGTH + STACK_TASK_REGISTER_STATE_LENGTH + STACK_POP_PRE_INCREMENT_ADJUSTMENT)];
}

/*| public_functions |*/

/*| public_privileged_functions |*/
