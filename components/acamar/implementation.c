/*| headers |*/
#include "rtos-acamar.h"

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/

/*| function_like_macros |*/

/*| functions |*/

/*| public_functions |*/
void
{{prefix_func}}yield_to(const {{prefix_type}}TaskId to) {{prefix_const}}REENTRANT
{
    rtos_internal_api_begin();
    const {{prefix_type}}TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
    rtos_internal_api_end();
}

/*| public_privileged_functions |*/
void
{{prefix_func}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), {{function}}, stack_{{idx}}, {{stack_size}});
    {{/tasks}}


    {{#memory_protection}}
    mpu_initialize();
    {{/memory_protection}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
