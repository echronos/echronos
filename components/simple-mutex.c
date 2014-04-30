/*| schema |*/
<entry name="mutexes" type="list" default="[]" auto_index_field="idx">
    <entry name="mutex" type="dict">
        <entry name="name" type="ident" />
    </entry>
</entry>

/*| public_headers |*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*| public_type_definitions |*/
{{#mutexes.length}}
typedef uint8_t {{prefix_type}}MutexId;
{{/mutexes.length}}

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#mutexes}}
#define {{prefix_const}}MUTEX_ID_{{name|u}} (({{prefix_type}}MutexId) UINT8_C({{idx}}))
{{/mutexes}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
{{#mutexes.length}}
void {{prefix_func}}mutex_lock({{prefix_type}}MutexId);
bool {{prefix_func}}mutex_try_lock({{prefix_type}}MutexId);
void {{prefix_func}}mutex_unlock({{prefix_type}}MutexId);
{{/mutexes.length}}

/*| headers |*/
{{#mutexes.length}}
#include <stdbool.h>
{{/mutexes.length}}

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/
{{#mutexes.length}}
struct mutex {
    bool locked;
};
{{/mutexes.length}}

/*| extern_definitions |*/

/*| function_definitions |*/

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
{{prefix_func}}mutex_lock(const {{prefix_type}}MutexId m)
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
