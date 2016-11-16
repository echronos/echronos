/*| headers |*/
{{#interrupt_events.length}}
#include <stdint.h>
#include <stdbool.h>
{{/interrupt_events.length}}

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
{{#interrupt_events.length}}
static void interrupt_event_process(void);
{{/interrupt_events.length}}
static inline void interrupt_event_wait(void);

/*| state |*/
{{#interrupt_events.length}}
volatile uint8_t {{prefix_func}}_internal_pending_interrupt_events;
{{/interrupt_events.length}}

/*| function_like_macros |*/
{{#interrupt_events.length}}
#define interrupt_application_event_check() ({{prefix_func}}_internal_pending_interrupt_events != 0)
{{/interrupt_events.length}}
{{^interrupt_events.length}}
#define interrupt_application_event_check() (false)
#define interrupt_event_process()
{{/interrupt_events.length}}

/*| functions |*/
{{#interrupt_events.length}}
static void
interrupt_event_process(void)
{
    if ({{prefix_func}}_internal_pending_interrupt_events != 0)
    {
        uint8_t processed_interrupts_mask = 0;

        {{prefix_type}}InterruptEventId event_id;
        for (event_id = 0; event_id < {{interrupt_events.length}}; event_id += 1)
        {
            const uint8_t mask = 1U << event_id;

            if (({{prefix_func}}_internal_pending_interrupt_events & mask) != 0)
            {
                processed_interrupts_mask |= mask;
                interrupt_event_handle(event_id);
            }
        }

        asm volatile("cli");
        {{prefix_func}}_internal_pending_interrupt_events &= ~processed_interrupts_mask;
        asm volatile("sei");
    }
}
{{/interrupt_events.length}}

static inline void
interrupt_event_wait(void)
{
    for (;;)
    {
        asm volatile("cli");
        if (interrupt_event_check())
        {
            asm volatile("sei");
            break;
        }
        else
        {
            /* TODO: enter power saving mode */
            asm volatile("sei");
            asm volatile("nop");
        }
    }
}

/*| public_functions |*/

/*| public_privileged_functions |*/
