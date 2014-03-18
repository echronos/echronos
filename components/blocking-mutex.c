/*| public_headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t {{prefix_type}}MutexId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#mutexes.length}}
#define {{prefix_const}}MUTEX_ID_ZERO (({{prefix_type}}MutexId) UINT8_C(0))
#define {{prefix_const}}MUTEX_ID_MAX (({{prefix_type}}MutexId) UINT8_C({{mutexes.length}} - 1))
{{#mutexes}}
#define {{prefix_const}}MUTEX_ID_{{name|u}} (({{prefix_type}}MutexId) UINT8_C({{idx}}))
{{/mutexes}}
{{/mutexes.length}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
{{#mutexes.length}}
void {{prefix_func}}mutex_lock({{prefix_type}}MutexId);
bool {{prefix_func}}mutex_try_lock({{prefix_type}}MutexId);
void {{prefix_func}}mutex_unlock({{prefix_type}}MutexId);
RtosTaskId {{prefix_func}}mutex_holder_get({{prefix_type}}MutexId);
{{/mutexes.length}}

/*| headers |*/

/*| object_like_macros |*/
{{#mutexes.length}}
#define MUTEX_ID_NONE ((MutexIdOption) UINT8_MAX)
{{/mutexes.length}}

/*| type_definitions |*/
typedef {{prefix_type}}MutexId MutexIdOption;

/*| structure_definitions |*/

struct mutex {
    TaskIdOption holder;
};

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/
{{#mutexes.length}}
static struct mutex mutexes[{{mutexes.length}}] = {
{{#mutexes}}
    {TASK_ID_NONE},
{{/mutexes}}
};
static MutexIdOption waiters[{{tasks.length}}] = {
{{#tasks}}
    MUTEX_ID_NONE,
{{/tasks}}
};
{{/mutexes.length}}

/*| function_like_macros |*/
{{#mutexes.length}}
#define assert_mutex_valid(mutex) api_assert(mutex < {{mutexes.length}}, ERROR_ID_INVALID_ID)
{{/mutexes.length}}

/*| functions |*/

/*| public_functions |*/
{{#mutexes.length}}
void
{{prefix_func}}mutex_lock(const {{prefix_type}}MutexId m)
{
    assert_mutex_valid(m);
    api_assert(mutexes[m].holder != get_current_task(), ERROR_ID_DEADLOCK);

    while (!{{prefix_func}}mutex_try_lock(m))
    {
        waiters[get_current_task()] = m;
        mutex_block_on(mutexes[m].holder);
    }
}

void
{{prefix_func}}mutex_unlock(const {{prefix_type}}MutexId m)
{
    {{prefix_type}}TaskId t;

    assert_mutex_valid(m);
    api_assert(mutexes[m].holder == get_current_task(), ERROR_ID_NOT_HOLDING_MUTEX);

    for (t = {{prefix_const}}TASK_ID_ZERO; t <= {{prefix_const}}TASK_ID_MAX; t++)
    {
        if (waiters[t] == m)
        {
            waiters[t] = MUTEX_ID_NONE;
            mutex_unblock(t);
        }
    }

    mutexes[m].holder = TASK_ID_NONE;
}

bool
{{prefix_func}}mutex_try_lock(const {{prefix_type}}MutexId m)
{
    assert_mutex_valid(m);

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

RtosTaskId
{{prefix_func}}mutex_holder_get(const {{prefix_type}}MutexId m)
{
    assert_mutex_valid(m);
    return mutexes[m].holder;
}
{{/mutexes.length}}
