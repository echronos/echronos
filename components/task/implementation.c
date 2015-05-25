/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/
#define TASK_ID_NONE ((TaskIdOption) UINT{{taskid_size}}_MAX)
#define current_task rtos_internal_current_task
#define tasks rtos_internal_tasks

/*| types |*/
typedef {{prefix_type}}TaskId TaskIdOption;

/*| structures |*/
struct task
{
    context_t ctx;
};

/*| extern_declarations |*/
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}

/*| function_declarations |*/
{{#internal_asserts}}
static {{prefix_type}}TaskId get_current_task_check(void);
{{/internal_asserts}}

/*| state |*/
{{prefix_type}}TaskId rtos_internal_current_task;
struct task rtos_internal_tasks[{{tasks.length}}];

/*| function_like_macros |*/
{{#internal_asserts}}
#define get_current_task() get_current_task_check()
{{/internal_asserts}}
{{^internal_asserts}}
#define get_current_task() current_task
{{/internal_asserts}}
#define get_task_context(task_id) &tasks[task_id].ctx
#define internal_assert_task_valid(task) internal_assert(task < {{tasks.length}}, ERROR_ID_INTERNAL_INVALID_ID)
#define assert_task_valid(task) api_assert(task < {{tasks.length}}, ERROR_ID_INVALID_ID)

/*| functions |*/
{{#internal_asserts}}
static {{prefix_type}}TaskId
get_current_task_check(void)
{
    internal_assert(current_task < {{tasks.length}}, ERROR_ID_INTERNAL_CURRENT_TASK_INVALID);
    return current_task;
}
{{/internal_asserts}}

/*| public_functions |*/
{{prefix_type}}TaskId
{{prefix_func}}task_current(void)
{
    return get_current_task();
}
