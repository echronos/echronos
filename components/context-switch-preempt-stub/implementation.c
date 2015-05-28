/*| headers |*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*| object_like_macros |*/
#define PREEMPTION_SUPPORT

/*| types |*/
typedef int context_t;

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/
void (*entry_point_ptr)(void);

/*| function_like_macros |*/
#define preempt_init()
#define context_init(ctx, fn, stack_base, stack_size) (entry_point_ptr = fn)
#define context_switch_first(to)
#define yield()
#define preempt_disable()
#define preempt_enable()
#define preempt_pend()
#define precondition_preemption_disabled()
#define postcondition_preemption_disabled()
#define postcondition_preemption_enabled()

/*| functions |*/

/*| public_functions |*/
