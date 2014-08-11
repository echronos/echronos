/*| dependencies |*/
ctxt_switch
documentation
stack
task

/*| schema |*/
<entry name="prefix" type="ident" optional="true" />

/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix_func}}yield_to({{prefix_type}}TaskId) {{prefix_const}}REENTRANT;
void {{prefix_func}}start(void);

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
