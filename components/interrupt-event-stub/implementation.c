/*| headers |*/
#include <stdbool.h>

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
static void interrupt_event_process(void);

/*| state |*/

/*| function_like_macros |*/
#define interrupt_application_event_check()
#define interrupt_event_wait()

/*| functions |*/
static void
interrupt_event_process(void)
{
{{#interrupt_events.length}}
    interrupt_event_handle(0);
{{/interrupt_events.length}}
}

/*| public_functions |*/
