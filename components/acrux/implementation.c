/*| headers |*/
#include "rtos-acrux.h"

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
static void interrupt_event_handle({{prefix_type}}InterruptEventId interrupt_event_id);

/*| state |*/

/*| function_like_macros |*/
#define interrupt_event_id_to_taskid(interrupt_event_id) (({{prefix_type}}TaskId)(interrupt_event_id))

/*| functions |*/
static void
interrupt_event_handle(const {{prefix_type}}InterruptEventId interrupt_event_id)
{
    sched_set_runnable(interrupt_event_id_to_taskid(interrupt_event_id));
}

/*| public_functions |*/
void
{{prefix_func}}yield_to(const {{prefix_type}}TaskId to) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

void
{{prefix_func}}yield(void) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TaskId to = interrupt_event_get_next();
    {{prefix_func}}yield_to(to);
}

void
{{prefix_func}}block(void) {{prefix_const}}REENTRANT
{
    sched_set_blocked(get_current_task());
    {{prefix_func}}yield();
}

void
{{prefix_func}}unblock(const {{prefix_type}}TaskId task)
{
    sched_set_runnable(task);
}

void
{{prefix_func}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), {{function}}, stack_{{idx}}, {{stack_size}});
    {{/tasks}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
