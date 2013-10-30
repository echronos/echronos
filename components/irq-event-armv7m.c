/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix_func}}interrupt_event_raise({{prefix_type}}InterruptEventId event);

/*| headers |*/
#include <stdint.h>
#include <stdbool.h>
#include "bitband.h"

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static void interrupt_event_process(void);
static inline bool interrupt_event_check(void);
static inline void interrupt_event_wait(void);

/*| state |*/
VOLATILE_BITBAND_VAR(uint32_t, interrupt_event);

/*| function_like_macros |*/

/*| functions |*/
static void
interrupt_event_process(void)
{
    uint32_t tmp = interrupt_event;
    while (tmp != 0)
    {
        {{prefix_type}}InterruptEventId i = __builtin_ffs(tmp) - 1;
        interrupt_event_bitband[i] = 0;
        handle_interrupt_event(i);
        tmp &= ~(1U << i);
    }
}

static inline bool
interrupt_event_check(void)
{
    return interrupt_event != 0;
}

static inline void
interrupt_event_wait(void)
{
    asm volatile("cpsid i");
    asm volatile("isb");
    if (!interrupt_event_check())
    {
        asm volatile("wfi");
    }
    asm volatile("cpsie i");
}

/*| public_functions |*/
void
{{prefix_func}}interrupt_event_raise({{prefix_type}}InterruptEventId interrupt_event_id)
{
    interrupt_event_bitband[interrupt_event_id] = 1;
}
