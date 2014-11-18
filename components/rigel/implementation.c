/*| headers |*/
#include "rtos-rigel.h"

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static void yield_to({{prefix_type}}TaskId to) {{prefix_const}}REENTRANT;
static void block(void) {{prefix_const}}REENTRANT;
static void unblock({{prefix_type}}TaskId task);

/*| state |*/
static {{prefix_type}}TimerId task_timers[{{tasks.length}}] = {
{{#tasks}}
    {{prefix_const}}TIMER_ID_{{timer.name|u}},
{{/tasks}}
};

/*| function_like_macros |*/
#define yield() {{prefix_func}}yield()
#define interrupt_event_id_to_taskid(interrupt_event_id) (({{prefix_type}}TaskId)(interrupt_event_id))
#define mutex_block_on(unused_task) {{prefix_func}}signal_wait({{prefix_const}}SIGNAL_ID__RTOS_UTIL)
#define mutex_unblock(task) {{prefix_func}}signal_send(task, {{prefix_const}}SIGNAL_ID__RTOS_UTIL)
#define message_queue_core_block() {{prefix_func}}signal_wait({{prefix_const}}SIGNAL_ID__TASK_TIMER)
#define message_queue_core_block_timeout(timeout) {{prefix_func}}sleep((timeout))
#define message_queue_core_unblock(task) {{prefix_func}}signal_send((task), {{prefix_const}}SIGNAL_ID__TASK_TIMER)
#define message_queue_core_is_unblocked(task) sched_runnable((task))

/*| functions |*/
static void
yield_to(const {{prefix_type}}TaskId to) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TaskId from = get_current_task();

    internal_assert(to < {{tasks.length}}, ERROR_ID_INTERNAL_INVALID_ID);

    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

static void
block(void) {{prefix_const}}REENTRANT
{
    sched_set_blocked(get_current_task());
    {{prefix_func}}yield();
}

static void
unblock(const {{prefix_type}}TaskId task)
{
    sched_set_runnable(task);
}

/* entry point trampolines */
{{#tasks}}
static void
entry_{{name}}(void)
{
    {{^start}}{{prefix_func}}signal_wait({{prefix_const}}SIGNAL_ID__RTOS_UTIL);{{/start}}
    {{function}}();

    api_error(ERROR_ID_TASK_FUNCTION_RETURNS);
}

{{/tasks}}

/*| public_functions |*/
void
{{prefix_func}}task_start(const {{prefix_type}}TaskId task)
{
    assert_task_valid(task);
    {{prefix_func}}signal_send(task, {{prefix_const}}SIGNAL_ID__RTOS_UTIL);
}

void
{{prefix_func}}yield(void) {{prefix_const}}REENTRANT
{
    {{prefix_type}}TaskId to = interrupt_event_get_next();
    yield_to(to);
}

void
{{prefix_func}}sleep(const {{prefix_type}}TicksRelative ticks) {{prefix_const}}REENTRANT
{
    {{prefix_func}}timer_oneshot(task_timers[get_current_task()], ticks);
    {{prefix_func}}signal_wait({{prefix_const}}SIGNAL_ID__TASK_TIMER);
}

void
{{prefix_func}}start(void)
{
    message_queue_init();

    {{#tasks}}
    context_init(get_task_context({{idx}}), entry_{{name}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
