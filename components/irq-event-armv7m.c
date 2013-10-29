/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix_func}}irq_event_raise({{prefix_type}}IrqEventId event);

/*| headers |*/
#include <stdint.h>
#include <stdbool.h>
#include "bitband.h"

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static void irq_event_process(void);
static inline bool irq_event_check(void);
static inline void irq_event_wait(void);

/*| state |*/
VOLATILE_BITBAND_VAR(uint32_t, irq_event);

/*| function_like_macros |*/

/*| functions |*/
static void
irq_event_process(void)
{
    uint32_t tmp = irq_event;
    while (tmp != 0)
    {
        {{prefix_type}}IrqEventId i = __builtin_ffs(tmp) - 1;
        irq_event_bitband[i] = 0;
        handle_irq_event(i);
        tmp &= ~(1U << i);
    }
}

static inline bool
irq_event_check(void)
{
    return irq_event != 0;
}

static inline void
irq_event_wait(void)
{
    asm volatile("cpsid i");
    asm volatile("isb");
    if (!irq_event_check())
    {
        asm volatile("wfi");
    }
    asm volatile("cpsie i");
}

/*| public_functions |*/
void
{{prefix_func}}irq_event_raise({{prefix_type}}IrqEventId irq_event_id)
{
    irq_event_bitband[irq_event_id] = 1;
}
