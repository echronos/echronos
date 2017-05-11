/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/
/* The TASK_ID_NONE and TASK_ID_END macros require some care:
 * - TASK_ID_NONE is a valid integer within the value range of the TaskIdOption/TaskId types.
 *   There is no fundamental safeguard against the application defining TASK_ID_NONE+1 tasks so that the last task
 *   receives a task ID that is numerically equal to TASK_ID_NONE.
 * - TASK_ID_END is of type integer, not TaskIdOption/TaskId.
 *   It may hold the value TASK_ID_MAX + 1 which potentially exceeds the valid value range of TaskIdOption/TaskId.
 *   It can therefore not necessarily be safely assigned to or cast to type TaskIdOption/TaskId. */
#define TASK_ID_NONE ((TaskIdOption) UINT{{taskid_size}}_MAX)
#define TASK_ID_END ({{tasks.length}})
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
/*@unused@ must be public so that packages/armv7m/ctxt-switch-preempt.s can access this symbol */
{{prefix_type}}TaskId rtos_internal_current_task;
/*@unused@ must be public so that packages/armv7m/ctxt-switch-preempt.s can access this symbol */
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
    {{prefix_type}}TaskId t;
    rtos_internal_api_begin();
    t = get_current_task();
    rtos_internal_api_end();
    return t;
}

/*| public_privileged_functions |*/
