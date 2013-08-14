/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint{{irqeventid_size}}_t IrqEventId;

/*| public_object_like_macros |*/
#define IRQ_EVENT_ID_C(x) ((IrqEventId) UINT{{irqeventid_size}}_C(x))
{{#irq_events}}
#define IRQ_EVENT_ID_{{name}} {{idx}}
{{/irq_events}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static TaskId irq_event_get_next(void);

/*| state |*/

/*| function_like_macros |*/

/*| functions |*/
static TaskId
irq_event_get_next(void)
{
    TaskIdOption next;

    for (;;)
    {
        irq_event_process();
        next = sched_get_next();
        if (next == TASK_ID_NONE)
        {
            irq_event_wait();
        }
        else
        {
            break;
        }
    }

    return next;
}

/*| public_functions |*/
