/*| headers |*/
#include <stddef.h>
#include "rtos-blocking-mutex-test.h"

/*| object_like_macros |*/
#define TASK_ID_NONE ((TaskIdOption) UINT8_MAX)

/*| types |*/
typedef {{prefix_type}}TaskId TaskIdOption;

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
static void mutex_core_block_on({{prefix_type}}TaskId task) {{prefix_const}}REENTRANT;
static void mutex_core_unblock({{prefix_type}}TaskId task);
static {{prefix_type}}TaskId get_current_task(void);

/*| state |*/
void (*block_on_ptr)({{prefix_type}}TaskId);
void (*unblock_ptr)({{prefix_type}}TaskId);
{{prefix_type}}TaskId (*get_current_task_ptr)(void);

/*| function_like_macros |*/
#define preempt_disable()
#define preempt_enable()
#define precondition_preemption_disabled()
#define postcondition_preemption_disabled()
#define api_assert(expression, error_id) do { } while(0)

/*| functions |*/
static void
mutex_core_block_on({{prefix_type}}TaskId task) {{prefix_const}}REENTRANT
{
    if (block_on_ptr != NULL)
    {
        block_on_ptr(task);
    }
}

static void
mutex_core_unblock({{prefix_type}}TaskId task)
{
    if (unblock_ptr != NULL)
    {
        unblock_ptr(task);
    }
}

static {{prefix_type}}TaskId
get_current_task(void)
{
    if (get_current_task_ptr != NULL)
    {
        return get_current_task_ptr();
    }
    return {{prefix_const}}TASK_ID_ZERO;
}

/*| public_functions |*/

struct mutex * pub_mutexes = mutexes;

void
pub_set_block_on_ptr(void (*y)({{prefix_type}}TaskId))
{
    block_on_ptr = y;
}

void
pub_set_unblock_ptr(void (*y)({{prefix_type}}TaskId))
{
    unblock_ptr = y;
}

void
pub_set_get_current_task_ptr({{prefix_type}}TaskId (*y)(void))
{
    get_current_task_ptr = y;
}

void pub_mutex_init(void)
{
    {{prefix_type}}MutexId mutex_id;
    /* For testing purposes we also reset all mutexes */
    for (mutex_id = {{prefix_const}}MUTEX_ID_ZERO; mutex_id <= {{prefix_const}}MUTEX_ID_MAX; mutex_id++)
    {
        mutexes[mutex_id].holder = TASK_ID_NONE;
    }
    block_on_ptr = NULL;
    unblock_ptr = NULL;
    get_current_task_ptr = NULL;
}
