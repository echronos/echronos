/*| headers |*/
#include <stdbool.h>

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/

/*| function_like_macros |*/
#define assert_timer_valid(timer) api_assert(timer_id < {{timers.length}}, ERROR_ID_INVALID_ID)

/*| functions |*/

/*| public_functions |*/
{{#timers.length}}
void
{{prefix_func}}timer_enable(const {{prefix_type}}TimerId timer_id)
{
    assert_timer_valid(timer_id);

    preempt_disable();

    timer_enable(timer_id);

    preempt_enable();
}

void
{{prefix_func}}timer_disable(const {{prefix_type}}TimerId timer_id)
{
    assert_timer_valid(timer_id);

    timers[timer_id].enabled = false;
}

void
{{prefix_func}}timer_oneshot(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}TicksRelative timeout)
{
    assert_timer_valid(timer_id);

    preempt_disable();

    timer_oneshot(timer_id, timeout);

    preempt_enable();
}

bool
{{prefix_func}}timer_check_overflow(const {{prefix_type}}TimerId timer_id)
{
    bool r;

    assert_timer_valid(timer_id);

    preempt_disable();

    r = timers[timer_id].overflow;
    timers[timer_id].overflow = false;

    preempt_enable();

    return r;
}

{{prefix_type}}TicksRelative
{{prefix_func}}timer_remaining(const {{prefix_type}}TimerId timer_id)
{
    {{prefix_type}}TicksRelative remaining;

    assert_timer_valid(timer_id);

    preempt_disable();

    remaining = timers[timer_id].enabled ? timers[timer_id].expiry - current_timeout() : 0;

    preempt_enable();

    return remaining;
}

/* Configuration functions */
void
{{prefix_func}}timer_reload_set(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}TicksRelative reload)
{
    assert_timer_valid(timer_id);

    timer_reload_set(timer_id, reload);
}

void
{{prefix_func}}timer_signal_set(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}TaskId task_id, const {{prefix_type}}SignalSet signal_set)
{
    assert_timer_valid(timer_id);

    preempt_disable();

    timers[timer_id].error_id = ERROR_ID_NONE;
    timers[timer_id].task_id = task_id;
    timers[timer_id].signal_set = signal_set;

    preempt_enable();
}

void
{{prefix_func}}timer_error_set(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}ErrorId error_id)
{
    assert_timer_valid(timer_id);

    timers[timer_id].error_id = error_id;
}
{{/timers.length}}
