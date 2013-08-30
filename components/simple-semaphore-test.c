/*| public_headers |*/

/*| public_type_definitions |*/
typedef uint8_t TaskId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/
#include "rtos-simple-semaphore-test.h"
#include <stdio.h>

/*| object_like_macros |*/
#define TASK_ID_ZERO 0
#define TASK_ID_MAX ({{tasks.length}} - 1)

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/
static void (*block_ptr)(void);
static void (*unblock_ptr)(TaskId);
static TaskId (*get_current_task_ptr)(void);

/*| function_like_macros |*/
#define preempt_enable()
#define preempt_disable()

/*| functions |*/
static void
_block(void)
{
    if (block_ptr != NULL)
    {
        block_ptr();
    }
}

static void
_unblock(TaskId task_id)
{
    if (unblock_ptr != NULL)
    {
        unblock_ptr(task_id);
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

struct semaphore * pub_semaphores = semaphores;
TaskId * pub_waiters = waiters;

void pub_set_block_ptr(void (*fn)(void))
{
    block_ptr = fn;
}

void pub_set_unblock_ptr(void (*fn)(TaskId))
{
    unblock_ptr = fn;
}

void pub_set_get_current_task_ptr(TaskId (*y)(void))
{
    get_current_task_ptr = y;
}

void pub_sem_init(void)
{
    SemId sem_id;
    sem_init();
    /* For testing purposes we also reset the value of all semaphores to zero */
    for (sem_id = SEM_ID_ZERO; sem_id <= SEM_ID_MAX; sem_id++)
    {
        semaphores[sem_id].value = SEM_VALUE_ZERO;
    }
    block_ptr = NULL;
    unblock_ptr = NULL;
    get_current_task_ptr = NULL;
}
