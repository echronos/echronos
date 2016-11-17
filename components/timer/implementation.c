/*| headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| object_like_macros |*/
{{#timers.length}}
#define TIMER_ID_ZERO (({{prefix_type}}TimerId) UINT8_C(0))
#define TIMER_ID_MAX (({{prefix_type}}TimerId) UINT8_C({{timers.length}} - 1))
{{/timers.length}}

/*| types |*/
typedef uint16_t TicksTimeout;

/*| structures |*/
{{#timers.length}}
struct timer
{
    bool enabled;
    bool overflow;
    TicksTimeout expiry;
    {{prefix_type}}TicksRelative reload;

    /*
     * when error_id is not ERROR_ID_NONE, the timer calls
     * the application error function with this error_id.
     */
    {{prefix_type}}ErrorId error_id;

    {{prefix_type}}TaskId task_id;
    {{prefix_type}}SignalSet signal_set;
};
{{/timers.length}}

/*| extern_declarations |*/

/*| function_declarations |*/
{{#timers.length}}
static void timer_process_one(struct timer *timer);
static void timer_enable({{prefix_type}}TimerId timer_id);
static void timer_oneshot({{prefix_type}}TimerId timer_id, {{prefix_type}}TicksRelative timeout);
{{/timers.length}}
static void timer_tick_process(void);

/*| state |*/
{{prefix_type}}TicksAbsolute {{prefix_func}}timer_current_ticks;
{{#timers.length}}
static struct timer timers[{{timers.length}}] = {
{{#timers}}
    {
        {{#enabled}}true{{/enabled}}{{^enabled}}false{{/enabled}},
        false,
        {{#enabled}}{{reload}}{{/enabled}}{{^enabled}}0{{/enabled}},
        {{reload}},
        {{error}},
        {{#task}}{{prefix_const}}TASK_ID_{{name|u}}{{/task}}{{^task}}TASK_ID_NONE{{/task}},
        {{#sig_set}}{{prefix_const}}SIGNAL_SET_{{.|u}}{{/sig_set}}{{^sig_set}}{{prefix_const}}SIGNAL_SET_EMPTY{{/sig_set}}
    },
{{/timers}}
};
{{/timers.length}}

/*| function_like_macros |*/
{{#timers.length}}
#define timer_expired(timer, timeout) ((timer)->enabled && (timer)->expiry == timeout)
#define timer_is_periodic(timer) ((timer)->reload > 0)
#define timer_reload_set(timer_id, ticks) timers[timer_id].reload = ticks
#define timer_disable(timer_id) timers[timer_id].enabled = false
#define current_timeout() ((TicksTimeout) {{prefix_func}}timer_current_ticks)
#define TIMER_PTR(timer_id) (&timers[timer_id])
{{/timers.length}}
#define assert_timer_valid(timer) api_assert(timer_id < {{timers.length}}, ERROR_ID_INVALID_ID)


/*| functions |*/
{{#timers.length}}
static void
timer_process_one(struct timer *const timer)
{
    precondition_preemption_disabled();

    if (timer_is_periodic(timer))
    {
        timer->expiry += timer->reload;
    }
    else
    {
        timer->enabled = false;
    }

    if (timer->error_id != ERROR_ID_NONE)
    {
        {{fatal_error}}(timer->error_id);
    }
    else
    {
        if (signal_pending(timer->task_id, timer->signal_set))
        {
            timer->overflow = true;
        }
        signal_send_set(timer->task_id, timer->signal_set);
    }

    postcondition_preemption_disabled();
}

static void
timer_enable(const {{prefix_type}}TimerId timer_id)
{
    precondition_preemption_disabled();

    if (timers[timer_id].reload == 0)
    {
        timer_process_one(&timers[timer_id]);
    }
    else
    {
        timers[timer_id].expiry = current_timeout() + timers[timer_id].reload;
        timers[timer_id].enabled = true;
    }

    postcondition_preemption_disabled();
}

static void
timer_oneshot(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}TicksRelative timeout)
{
    precondition_preemption_disabled();

    timer_reload_set(timer_id, timeout);
    timer_enable(timer_id);
    timer_reload_set(timer_id, 0);

    postcondition_preemption_disabled();
}
{{/timers.length}}

static void
timer_tick_process(void)
{
    precondition_preemption_disabled();
    {
        const uint8_t pending_ticks = timer_pending_ticks_get_and_clear_atomically();

        if (pending_ticks > 1)
        {
            {{fatal_error}}(ERROR_ID_TICK_OVERFLOW);
        }

        if (pending_ticks != 0)
        {
            {{#timers.length}}
            {{prefix_type}}TimerId timer_id;
            struct timer *timer;
            TicksTimeout timeout;
            {{/timers.length}}

            {{prefix_func}}timer_current_ticks++;

            {{#timers.length}}
            timeout = current_timeout();

            for (timer_id = TIMER_ID_ZERO; timer_id <= TIMER_ID_MAX; timer_id++)
            {
                timer = TIMER_PTR(timer_id);
                if (timer_expired(timer, timeout))
                {
                    timer_process_one(timer);
                }
            }
            {{/timers.length}}
        }
    }
    postcondition_preemption_disabled();
}

/*| public_functions |*/
void
{{prefix_func}}sleep(const {{prefix_type}}TicksRelative ticks) {{prefix_const}}REENTRANT
{
    preempt_disable();

    timer_oneshot(task_timers[get_current_task()], ticks);
    signal_wait({{prefix_const}}SIGNAL_ID__TASK_TIMER);

    preempt_enable();
}

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

    timer_disable(timer_id);
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
    assert_task_valid(task_id);
    api_assert(signal_set != {{prefix_const}}SIGNAL_SET_EMPTY, ERROR_ID_TIMER_SIGNAL_SET_IS_EMPTY);

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

/*| public_privileged_functions |*/
