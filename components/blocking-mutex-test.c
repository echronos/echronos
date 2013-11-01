/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t {{prefix_type}}TaskId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define {{prefix_const}}TASK_ID_ZERO (({{prefix_type}}TaskId) UINT8_C(0))
#define {{prefix_const}}TASK_ID_MAX (({{prefix_type}}TaskId)UINT8_C({{tasks.length}} - 1))

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/
#include <stddef.h>
#include "rtos-blocking-mutex-test.h"

/*| object_like_macros |*/
#define TASK_ID_NONE ((TaskIdOption) UINT8_MAX)

/*| type_definitions |*/
typedef {{prefix_type}}TaskId TaskIdOption;

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static void mutex_block_on({{prefix_type}}TaskId task);
static void mutex_unblock({{prefix_type}}TaskId task);
static {{prefix_type}}TaskId get_current_task(void);

/*| state |*/
void (*block_on_ptr)({{prefix_type}}TaskId);
void (*unblock_ptr)({{prefix_type}}TaskId);
{{prefix_type}}TaskId (*get_current_task_ptr)(void);

/*| function_like_macros |*/

/*| functions |*/
static void
mutex_block_on({{prefix_type}}TaskId task)
{
    if (block_on_ptr != NULL)
    {
        block_on_ptr(task);
    }
}

static void
mutex_unblock({{prefix_type}}TaskId task)
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
