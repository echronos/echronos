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
{{#mutex.stats}}
extern bool {{prefix_func}}mutex_stats_enabled;
{{/mutex.stats}}

/*| public_function_definitions |*/
{{#mutexes.length}}
void {{prefix_func}}mutex_lock({{prefix_type}}MutexId);
bool {{prefix_func}}mutex_try_lock({{prefix_type}}MutexId);
void {{prefix_func}}mutex_unlock({{prefix_type}}MutexId);
{{#mutex.stats}}
void {{prefix_func}}mutex_stats_clear(void);
{{/mutex.stats}}
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
{{#mutex.stats}}
struct mutex_stat {
    uint32_t request_counter;
    uint32_t uncontended_counter;
    uint32_t contended_counter;
    {{prefix_type}}TicksRelative max_wait_time;
};
{{/mutex.stats}}

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
{{#mutex.stats}}
bool {{prefix_func}}mutex_stats_enabled;
static struct mutex_stat mutex_stats[{{mutexes.length}}];
static {{prefix_type}}TicksAbsolute mutex_stats_wait_start_ticks[{{tasks.length}}];
{{/mutex.stats}}
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
{{#mutex.stats}}

    mutex_stats_wait_start_ticks[get_current_task()] = {{prefix_func}}timer_current_ticks;
{{/mutex.stats}}

    while (!{{prefix_func}}mutex_try_lock(m))
    {
        waiters[get_current_task()] = m;
        mutex_block_on(mutexes[m].holder);
    }
{{#mutex.stats}}

    mutex_stats_wait_start_ticks[get_current_task()] = 0;
{{/mutex.stats}}
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
{{#mutex.stats}}

    if ({{prefix_func}}mutex_stats_enabled)
    {
        mutex_stats[m].request_counter += 1;
    }
    if (!mutex_stats_wait_start_ticks[get_current_task()])
    {
        mutex_stats_wait_start_ticks[get_current_task()] = {{prefix_func}}timer_current_ticks;
    }
{{/mutex.stats}}

    if (mutexes[m].holder != TASK_ID_NONE)
    {
{{#mutex.stats}}
        if ({{prefix_func}}mutex_stats_enabled)
        {
            mutex_stats[m].contended_counter += 1;
        }
{{/mutex.stats}}
        return false;
    }
    else
    {
        mutexes[m].holder = get_current_task();
{{#mutex.stats}}
        if ({{prefix_func}}mutex_stats_enabled)
        {
            {{prefix_type}}TicksRelative wait_time = {{prefix_func}}timer_current_ticks - mutex_stats_wait_start_ticks[get_current_task()];
            if (wait_time > mutex_stats[m].max_wait_time)
            {
                mutex_stats[m].max_wait_time = wait_time;
            }
            mutex_stats[m].uncontended_counter += 1;
        }
{{/mutex.stats}}
        return true;
    }
}

{{#mutex.stats}}
void {{prefix_func}}mutex_stats_clear(void)
{
    /* memset would be preferable, but string.h is not available on all platforms */
    uint8_t mutex_index;
    for (mutex_index = 0; mutex_index < {{mutexes.length}}; mutex_index += 1)
    {
        mutex_stats[mutex_index].request_counter = 0;
        mutex_stats[mutex_index].uncontended_counter = 0;
        mutex_stats[mutex_index].contended_counter = 0;
        mutex_stats[mutex_index].max_wait_time = 0;
    }
}
{{/mutex.stats}}
{{/mutexes.length}}
