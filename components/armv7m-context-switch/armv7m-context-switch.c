/*| headers |*/
#include <stdint.h>

/*| public_type_definitions |*/

/*| public_macros |*/

/*| public_function_definitions |*/

/*| object_like_macros |*/
#define CONTEXT_SIZE 10
#define CONTEXT_V1_IDX 0
#define CONTEXT_V2_IDX 1
#define CONTEXT_V3_IDX 2
#define CONTEXT_V4_IDX 3
#define CONTEXT_V5_IDX 4
#define CONTEXT_V6_IDX 5
#define CONTEXT_V7_IDX 6
#define CONTEXT_V8_IDX 7
#define CONTEXT_IP_IDX 8
#define CONTEXT_PC_IDX 9

/*| type_definitions |*/
typedef uint32_t* context_t;

/*| structure_definitions |*/

/*| extern_definitions |*/
extern void armv7m_context_switch(context_t *, context_t *);
extern void armv7m_context_switch_first(context_t *);
extern void armv7m_trampoline(void);

/*| function_definitions |*/
static void context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size);

/*| state |*/

/*| function_like_macros |*/
#define context_switch(from, to) armv7m_context_switch(to, from)
#define context_switch_first(to) armv7m_context_switch_first(to)

/*| functions |*/
static void
context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size)
{
    uint32_t *context;
    context = stack_base + stack_size - CONTEXT_SIZE;
    context[CONTEXT_V1_IDX] = (uint32_t) fn;
    context[CONTEXT_PC_IDX] = (uint32_t) armv7m_trampoline;
    *ctx = context;
}

/*| public_functions |*/
