/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix_func}}yield(void) {{prefix_const}}REENTRANT;
void {{prefix_func}}start(void);

/*| headers |*/
#include "rtos-kraz.h"

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static void _yield_to(const {{prefix_type}}TaskId to) {{prefix_const}}REENTRANT;
static void _block(void) {{prefix_const}}REENTRANT;
static void _unblock(const {{prefix_type}}TaskId task);

/*| state |*/

/*| function_like_macros |*/
#define preempt_disable()
#define preempt_enable()

/*| functions |*/
static void
_yield_to(const {{prefix_type}}TaskId to) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

static void
_block(void) {{prefix_const}}REENTRANT
{
    sched_set_blocked(get_current_task());
    {{prefix_func}}yield();
}

static void
_unblock(const {{prefix_type}}TaskId task)
{
    sched_set_runnable(task);
}

/*| public_functions |*/
void
{{prefix_func}}yield(void) {{prefix_const}}REENTRANT
{
    {{prefix_type}}TaskId to = sched_get_next();
    _yield_to(to);
}

void
{{prefix_func}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), {{function}}, stack_{{idx}}, {{stack_size}});
    {{/tasks}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
