/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/

/*| type_definitions |*/
typedef int context_t;

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static void context_switch(context_t *from, context_t *to);

/*| state |*/
void (*entry_point_ptr)(void);

/*| function_like_macros |*/
#define context_init(ctx, fn, stack_base, stack_size) (entry_point_ptr = fn)
#define context_switch_first(to)

/*| functions |*/
static void
context_switch(context_t *from, context_t *to)
{
    from = from;
    to = to;
}

/*| public_functions |*/
