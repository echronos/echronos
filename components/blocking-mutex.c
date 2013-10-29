/*| public_headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t MutexId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define MUTEX_ID_ZERO ((MutexId) UINT8_C(0))
#define MUTEX_ID_MAX ((MutexId) UINT8_C({{mutexes.length}} - 1))
{{#mutexes}}
#define MUTEX_ID_{{name|u}} ((MutexId) UINT8_C({{idx}}))
{{/mutexes}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix_func}}mutex_lock(MutexId);
bool {{prefix_func}}mutex_try_lock(MutexId);
void {{prefix_func}}mutex_unlock(MutexId);

/*| headers |*/

/*| object_like_macros |*/
#define MUTEX_ID_NONE ((MutexIdOption) UINT8_MAX)

/*| type_definitions |*/
typedef MutexId MutexIdOption;

/*| structure_definitions |*/

struct mutex {
    TaskIdOption holder;
};

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/
static struct mutex mutexes[{{mutexes.length}}];
static MutexIdOption waiters[{{tasks.length}}];

/*| function_like_macros |*/

/*| functions |*/
static void
mutex_init(void)
{
    TaskId t;

    for (t = TASK_ID_ZERO; t <= TASK_ID_MAX; t++)
    {
        waiters[t] = MUTEX_ID_NONE;
    }
}

/*| public_functions |*/
void
{{prefix_func}}mutex_lock(const MutexId m)
{
    while (!{{prefix_func}}mutex_try_lock(m))
    {
        waiters[get_current_task()] = m;
        block_on(mutexes[m].holder);
    }
}

void
{{prefix_func}}mutex_unlock(const MutexId m)
{
    TaskId t;

    for (t = TASK_ID_ZERO; t <= TASK_ID_MAX; t++)
    {
        if (waiters[t] == m)
        {
            waiters[t] = MUTEX_ID_NONE;
            unblock(t);
        }
    }

    mutexes[m].holder = TASK_ID_NONE;
}

bool
{{prefix_func}}mutex_try_lock(const MutexId m)
{
    if (mutexes[m].holder != TASK_ID_NONE)
    {
        return false;
    }
    else
    {
        mutexes[m].holder = get_current_task();
        return true;
    }
}
