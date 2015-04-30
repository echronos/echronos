/*| headers |*/
{{#mutexes.length}}
#include <stdbool.h>
{{/mutexes.length}}

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/
{{#mutexes.length}}
struct mutex {
    bool locked;
};
{{/mutexes.length}}

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/
{{#mutexes.length}}
static struct mutex mutexes[{{mutexes.length}}];
{{/mutexes.length}}

/*| function_like_macros |*/
#define assert_mutex_valid(mutex) api_assert(mutex < {{mutexes.length}}, ERROR_ID_INVALID_ID)

/*| functions |*/
{{#mutexes.length}}
static bool
internal_mutex_try_lock(const {{prefix_type}}MutexId m)
{
    if (mutexes[m].locked)
    {
        return false;
    }
    else
    {
        mutexes[m].locked = true;
        return true;
    }
}
{{/mutexes.length}}

/*| public_functions |*/
{{#mutexes.length}}
void
{{prefix_func}}mutex_lock(const {{prefix_type}}MutexId m) {{prefix_const}}REENTRANT
{
    assert_mutex_valid(m);

    preempt_disable();

    while (!internal_mutex_try_lock(m))
    {
        {{prefix_func}}yield();
    }

    preempt_enable();
}

void
{{prefix_func}}mutex_unlock(const {{prefix_type}}MutexId m)
{
    assert_mutex_valid(m);

    /* Note: assumes writing a single word is atomic */
    mutexes[m].locked = false;
}

bool
{{prefix_func}}mutex_try_lock(const {{prefix_type}}MutexId m)
{
    bool r;

    assert_mutex_valid(m);

    preempt_disable();
    r = internal_mutex_try_lock(m);
    preempt_enable();

    return r;
}
{{/mutexes.length}}
