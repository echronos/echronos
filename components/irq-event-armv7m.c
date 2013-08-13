/*| headers |*/
#include "bitband.h"

/*| public_type_definitions |*/

/*| public_macros |*/

/*| object_like_macros |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

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
        IrqEventId i = __builtin_ffs(tmp) - 1;
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
{{prefix}}irq_event_raise(IrqEventId irq_event_id)
{
    irq_event_bitband[irq_event_id] = 1;
}
