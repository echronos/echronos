/*| headers |*/
#include "rtos-acamar.h"

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/

/*| function_like_macros |*/
#define preempt_disable()
#define preempt_enable()

/*| functions |*/

/*| public_functions |*/
void
{{prefix_func}}yield_to(const {{prefix_type}}TaskId to) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

void
{{prefix_func}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), {{function}}, stack_{{idx}}, {{stack_size}});
    {{/tasks}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
