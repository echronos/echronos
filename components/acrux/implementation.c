/*| headers |*/
#include "rtos-acrux.h"

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
extern void rtos_internal_interrupts_disable(void);
extern void rtos_internal_interrupts_enable(void);
extern void rtos_internal_interrupts_wait(void);
static void handle_interrupt_event({{prefix_type}}InterruptEventId interrupt_event_id);

/*| state |*/

/*| function_like_macros |*/
#define interrupts_disable() rtos_internal_interrupts_disable()
#define interrupts_enable() rtos_internal_interrupts_enable()
#define interrupts_wait() rtos_internal_interrupts_wait()
#define interrupt_event_id_to_taskid(interrupt_event_id) (({{prefix_type}}TaskId)(interrupt_event_id))

/*| functions |*/
static void
handle_interrupt_event({{prefix_type}}InterruptEventId interrupt_event_id)
{
    sched_set_runnable(interrupt_event_id_to_taskid(interrupt_event_id));
}

/*| public_functions |*/
void
{{prefix_func}}yield_to({{prefix_type}}TaskId to) {{prefix_const}}REENTRANT
{
    {{prefix_type}}TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

void
{{prefix_func}}yield(void) {{prefix_const}}REENTRANT
{
    {{prefix_type}}TaskId to = interrupt_event_get_next();
    {{prefix_func}}yield_to(to);
}

void
{{prefix_func}}block(void) {{prefix_const}}REENTRANT
{
    sched_set_blocked(get_current_task());
    {{prefix_func}}yield();
}

void
{{prefix_func}}unblock({{prefix_type}}TaskId task)
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
