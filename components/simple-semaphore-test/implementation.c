/*| headers |*/
#include <stdio.h>
#include <stddef.h>
#include "rtos-simple-semaphore-test.h"

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/
static void (*block_ptr)(void);
static void (*unblock_ptr)({{prefix_type}}TaskId);
static {{prefix_type}}TaskId (*get_current_task_ptr)(void);

/*| function_like_macros |*/
#define sem_core_block() block()
#define sem_core_unblock(task_id) unblock(task_id)
#define api_assert(expression, error_id) do { } while(0)

/*| functions |*/
static void
block(void) {{prefix_const}}REENTRANT
{
    if (block_ptr != NULL)
    {
        block_ptr();
    }
}

static void
unblock({{prefix_type}}TaskId task_id)
{
    if (unblock_ptr != NULL)
    {
        unblock_ptr(task_id);
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

struct semaphore * pub_semaphores = semaphores;
{{prefix_type}}TaskId * pub_waiters = sem_waiters;

void pub_set_block_ptr(void (*fn)(void))
{
    block_ptr = fn;
}

void pub_set_unblock_ptr(void (*fn)({{prefix_type}}TaskId))
{
    unblock_ptr = fn;
}

void pub_set_get_current_task_ptr({{prefix_type}}TaskId (*y)(void))
{
    get_current_task_ptr = y;
}

void pub_sem_init(void)
{
    {{prefix_type}}SemId sem_id;
    sem_init();
    /* For testing purposes we also reset the value of all semaphores to zero */
    for (sem_id = {{prefix_const}}SEM_ID_ZERO; sem_id <= {{prefix_const}}SEM_ID_MAX; sem_id++)
    {
        semaphores[sem_id].value = SEM_VALUE_ZERO;
    }
    block_ptr = NULL;
    unblock_ptr = NULL;
    get_current_task_ptr = NULL;
}
