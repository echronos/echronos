/*| headers |*/
#include "rtos-acamar.h"

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/
{{#profiling}}
{{#profiling.hook_for_task_switch}}
extern void {{hook_for_task_switch}}({{prefix_type}}TaskId from, {{prefix_type}}TaskId to);
{{/profiling.hook_for_task_switch}}
{{/profiling}}

/*| function_declarations |*/

/*| state |*/

/*| function_like_macros |*/

/*| functions |*/

/*| public_functions |*/
void
{{prefix_func}}yield_to(const {{prefix_type}}TaskId to) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TaskId from = get_current_task();

    {{#profiling}}
    {{#profiling.hook_for_task_switch}}
    {{hook_for_task_switch}}(from, to);
    {{/profiling.hook_for_task_switch}}
    {{/profiling}}

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
