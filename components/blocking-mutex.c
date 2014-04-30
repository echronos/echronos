/*| schema |*/
<entry name="mutexes" type="list" default="[]" auto_index_field="idx">
    <entry name="mutex" type="dict">
        <entry name="name" type="ident" />
    </entry>
</entry>
<entry name="mutex" type="dict" optional="true">
    <entry name="stats" type="bool" optional="true" default="false" />
</entry>

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
RtosTaskId {{prefix_func}}mutex_holder_get({{prefix_type}}MutexId);
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
    uint32_t mutex_lock_counter;
    uint32_t mutex_lock_contended_counter;
    {{prefix_type}}TicksRelative mutex_lock_max_wait_time;
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
{{#mutex.stats}}
    bool contended = false;
    const {{prefix_type}}TicksAbsolute wait_start_ticks = {{prefix_func}}timer_current_ticks;

{{/mutex.stats}}
    assert_mutex_valid(m);
    api_assert(mutexes[m].holder != get_current_task(), ERROR_ID_DEADLOCK);

    while (!{{prefix_func}}mutex_try_lock(m))
    {
{{#mutex.stats}}
        contended = true;
{{/mutex.stats}}
        waiters[get_current_task()] = m;
        mutex_block_on(mutexes[m].holder);
    }
{{#mutex.stats}}

    if ({{prefix_func}}mutex_stats_enabled)
    {
        mutex_stats[m].mutex_lock_counter += 1;
        if (contended)
        {
            {{prefix_type}}TicksRelative wait_time = {{prefix_func}}timer_current_ticks - wait_start_ticks;

            mutex_stats[m].mutex_lock_contended_counter += 1;
            if (wait_time > mutex_stats[m].mutex_lock_max_wait_time)
            {
                mutex_stats[m].mutex_lock_max_wait_time = wait_time;
            }
        }
    }
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

{{#mutex.stats}}
void {{prefix_func}}mutex_stats_clear(void)
{
    /* memset would be preferable, but string.h is not available on all platforms */
    uint8_t mutex_index;
    for (mutex_index = 0; mutex_index < {{mutexes.length}}; mutex_index += 1)
    {
        mutex_stats[mutex_index].mutex_lock_counter = 0;
        mutex_stats[mutex_index].mutex_lock_contended_counter = 0;
        mutex_stats[mutex_index].mutex_lock_max_wait_time = 0;
    }
}
{{/mutex.stats}}
{{/mutexes.length}}
