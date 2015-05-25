/*| headers |*/
#include "rtos-kraz.h"

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
static void yield_to({{prefix_type}}TaskId to) {{prefix_const}}REENTRANT;
static void block(void) {{prefix_const}}REENTRANT;
static void unblock({{prefix_type}}TaskId task);

/*| state |*/

/*| function_like_macros |*/
#define yield() {{prefix_func}}yield()

/*| functions |*/
static void
yield_to(const {{prefix_type}}TaskId to) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TaskId from = get_current_task();
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

/*| public_functions |*/
void
{{prefix_func}}yield(void) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TaskId to = sched_get_next();
    yield_to(to);
}

void
{{prefix_func}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), {{function}}, stack_{{idx}}, {{stack_size}});
    {{/tasks}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
