/*| headers |*/
{{#interrupt_events.length}}
#include <stdint.h>
#include <stdbool.h>
#include "bitband.h"
{{/interrupt_events.length}}

/*| object_like_macros |*/
#define interrupt_event rtos_internal_interrupt_event
#define interrupt_event_bitband rtos_internal_interrupt_event_bitband

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
static void interrupt_event_process(void);
static inline bool interrupt_application_event_check(void);
static inline void interrupt_event_wait(void);

/*| state |*/
{{#interrupt_events.length}}
VOLATILE_BITBAND_VAR(uint32_t, rtos_internal_interrupt_event);
{{/interrupt_events.length}}

/*| function_like_macros |*/

/*| functions |*/
static void
interrupt_event_process(void)
{
{{#interrupt_events.length}}
    uint32_t tmp = interrupt_event;
    while (tmp != 0)
    {
        const {{prefix_type}}InterruptEventId i = __builtin_ffs(tmp) - 1;
        interrupt_event_bitband[i] = 0;
        interrupt_event_handle(i);
        tmp &= ~(1U << i);
    }
{{/interrupt_events.length}}
}

static inline bool
interrupt_application_event_check(void)
{
{{#interrupt_events.length}}
    return interrupt_event != 0;
{{/interrupt_events.length}}
{{^interrupt_events.length}}
    return false;
{{/interrupt_events.length}}
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

/*| public_privileged_functions |*/
{{#interrupt_events.length}}
void
{{prefix_func}}interrupt_event_raise(const {{prefix_type}}InterruptEventId interrupt_event_id)
{
    interrupt_event_bitband[interrupt_event_id] = 1;
}
{{/interrupt_events.length}}
