/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint{{taskid_size}}_t {{prefix_type}}TaskId;
typedef uint8_t {{prefix_type}}ErrorId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define {{prefix_const}}TASK_ID_ZERO (({{prefix_type}}TaskId) UINT{{taskid_size}}_C(0))
#define {{prefix_const}}TASK_ID_MAX (({{prefix_type}}TaskId)UINT{{taskid_size}}_C({{tasks.length}} - 1))
{{#tasks}}
#define {{prefix_const}}TASK_ID_{{name|u}} (({{prefix_type}}TaskId) UINT{{taskid_size}}_C({{idx}}))
{{/tasks}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix_func}}start(void);
void {{prefix_func}}yield(void);
void {{prefix_func}}sleep({{prefix_type}}TicksRelative ticks);
void {{prefix_func}}task_start({{prefix_type}}TaskId task);
{{prefix_type}}TaskId {{prefix_func}}task_current(void);

/*| headers |*/
#include <stdint.h>
#include "rtos-rigel.h"

/*| object_like_macros |*/
#define TASK_ID_NONE ((TaskIdOption) UINT{{taskid_size}}_MAX)

#define ERROR_ID_NONE (({{prefix_type}}ErrorId) UINT8_C(0))
#define ERROR_ID_TICK_OVERFLOW (({{prefix_type}}ErrorId) UINT8_C(1))
#define ERROR_ID_NO_TASKS_AVAILABLE (({{prefix_type}}ErrorId) UINT8_C(2))

/*| type_definitions |*/
typedef {{prefix_type}}TaskId TaskIdOption;

/*| structure_definitions |*/
struct task
{
    context_t ctx;
};

struct interrupt_event_handler {
    {{prefix_type}}TaskId task;
    {{prefix_type}}SignalSet sig_set;
};

/*| extern_definitions |*/
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}
extern void {{fatal_error}}({{prefix_type}}ErrorId error_id);

/*| function_definitions |*/
static void _yield_to({{prefix_type}}TaskId to);
static void _block(void);
static void _unblock({{prefix_type}}TaskId task);
{{#interrupt_events.length}}
static void handle_interrupt_event({{prefix_type}}InterruptEventId interrupt_event_id);
{{/interrupt_events.length}}


/*| state |*/
static {{prefix_type}}TaskId current_task;
static struct task tasks[{{tasks.length}}];
static {{prefix_type}}TimerId task_timers[{{tasks.length}}] = {
{{#tasks}}
    {{prefix_const}}TIMER_ID_{{timer.name|u}},
{{/tasks}}
};

struct interrupt_event_handler interrupt_events[{{interrupt_events.length}}] = {
{{#interrupt_events}}
    { {{prefix_const}}TASK_ID_{{task.name|u}}, {{prefix_const}}SIGNAL_SET_{{sig_set|u}} },
{{/interrupt_events}}
};

/*| function_like_macros |*/
#define preempt_disable()
#define preempt_enable()
#define get_current_task() current_task
#define get_task_context(task_id) &tasks[task_id].ctx
#define interrupt_event_id_to_taskid(interrupt_event_id) (({{prefix_type}}TaskId)(interrupt_event_id))

/*| functions |*/
static void
_yield_to({{prefix_type}}TaskId to)
{
    {{prefix_type}}TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

static void
_block(void)
{
    sched_set_blocked(get_current_task());
    {{prefix_func}}yield();
}

static void
_unblock({{prefix_type}}TaskId task)
{
    sched_set_runnable(task);
}

{{#interrupt_events.length}}
static void
handle_interrupt_event({{prefix_type}}InterruptEventId interrupt_event_id)
{
    {{prefix_type}}TaskId task = interrupt_events[interrupt_event_id].task;
    {{prefix_type}}SignalSet sig_set = interrupt_events[interrupt_event_id].sig_set;

    {{prefix_func}}signal_send_set(task, sig_set);
}
{{/interrupt_events.length}}

/* entry point trampolines */
{{#tasks}}
void _task_entry_{{name}}(void)
{
    {{^start}}{{prefix_func}}signal_wait({{prefix_const}}SIGNAL_ID__RTOS_UTIL);{{/start}}
    {{function}}();
}

{{/tasks}}

/*| public_functions |*/
{{prefix_type}}TaskId
{{prefix_func}}task_current(void)
{
    return current_task;
}

void
{{prefix_func}}task_start({{prefix_type}}TaskId task)
{
    {{prefix_func}}signal_send(task, {{prefix_const}}SIGNAL_ID__RTOS_UTIL);
}

void
{{prefix_func}}yield(void)
{
    {{prefix_type}}TaskId to = interrupt_event_get_next();
    _yield_to(to);
}

void
{{prefix_func}}sleep({{prefix_type}}TicksRelative ticks)
{
    {{prefix_func}}timer_oneshot(task_timers[get_current_task()], ticks);
    {{prefix_func}}signal_wait({{prefix_const}}SIGNAL_ID__TASK_TIMER);
}

void
{{prefix_func}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), _task_entry_{{name}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
