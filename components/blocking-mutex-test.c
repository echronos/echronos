/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t TaskId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define TASK_ID_ZERO ((TaskId) 0)
#define TASK_ID_MAX ((TaskId)({{tasks.length}} - 1))

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/
#include "rtos-blocking-mutex-test.h"

/*| object_like_macros |*/
#define TASK_ID_NONE ((TaskIdOption) UINT8_MAX)

/*| type_definitions |*/
typedef TaskId TaskIdOption;

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/
static void block_on(TaskId task);
static void unblock(TaskId task);
static TaskId get_current_task(void);

/*| state |*/
void (*block_on_ptr)(TaskId);
void (*unblock_ptr)(TaskId);
TaskId (*get_current_task_ptr)(void);

/*| function_like_macros |*/

/*| functions |*/
static void
block_on(TaskId task)
{
    if (block_on_ptr != NULL)
    {
        block_on_ptr(task);
    }
}

static void
unblock(TaskId task)
{
    if (unblock_ptr != NULL)
    {
        unblock_ptr(task);
    }
}

static TaskId
get_current_task(void)
{
    if (get_current_task_ptr != NULL)
    {
        return get_current_task_ptr();
    }
    return TASK_ID_ZERO;
}

/*| public_functions |*/

struct mutex * pub_mutexes = mutexes;

void
pub_set_block_on_ptr(void (*y)(TaskId))
{
    block_on_ptr = y;
}

void
pub_set_unblock_ptr(void (*y)(TaskId))
{
    unblock_ptr = y;
}

void
pub_set_get_current_task_ptr(TaskId (*y)(void))
{
    get_current_task_ptr = y;
}

void pub_mutex_init(void)
{
    MutexId mutex_id;
    mutex_init();
    /* For testing purposes we also reset all mutexes */
    for (mutex_id = MUTEX_ID_ZERO; mutex_id <= MUTEX_ID_MAX; mutex_id++)
    {
        mutexes[mutex_id].holder = TASK_ID_NONE;
    }
    block_on_ptr = NULL;
    unblock_ptr = NULL;
    get_current_task_ptr = NULL;
}
