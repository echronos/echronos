/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t TimerId;
typedef uint32_t TicksAbsolute;
typedef uint16_t TicksRelative;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
#define TIMER_ID_C(x) ((TimerId) UINT8_C(x))
{{#timers}}
#define TIMER_ID_{{name|u}} SIGNAL_ID_C({{idx}})
{{/timers}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/
extern TicksAbsolute {{prefix}}timer_current_ticks;

/*| public_function_definitions |*/
void {{prefix}}timer_enable(TimerId timer_id);
void {{prefix}}timer_disable(TimerId timer_id);
void {{prefix}}timer_oneshot(TimerId timer_id, TicksRelative timeout);
bool {{prefix}}timer_check_overflow(TimerId timer_id);
TicksRelative {{prefix}}timer_remaining(TimerId timer_id);
void {{prefix}}timer_reload_set(TimerId timer_id, TicksRelative reload);
void {{prefix}}timer_reload_clear(TimerId timer_id);
void {{prefix}}timer_signal_set(TimerId timer_id, TaskId task_id, SignalSet signal_set);

/*| headers |*/
#include <stdbool.h>

/*| object_like_macros |*/
#define TIMER_ID_ZERO TIMER_ID_C(0)
#define TIMER_ID_MAX TIMER_ID_C({{timers.length}} - 1U)

/*| type_definitions |*/
typedef uint16_t TicksTimeout;

/*| structure_definitions |*/
struct timer
{
    bool enabled;
    bool overflow;
    TicksTimeout expiry;
    TicksRelative reload;

    /*
     * when error_id is not ERROR_ID_NONE, the timer calls
     * the application error function with this error_id.
     */
    ErrorId error_id;

    TaskId task_id;
    SignalSet signal_set;
};

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/
TicksAbsolute {{prefix}}timer_current_ticks;
static struct timer timers[{{timers.length}}] = {
{{#timers}}
    {
        {{#enabled}}true{{/enabled}}{{^enabled}}false{{/enabled}},
        false,
        {{#enabled}}{{reload}}{{/enabled}}{{^enabled}}0{{/enabled}},
        {{reload}},
        {{error}},
        {{#task}}TASK_ID_{{name|u}}{{/task}}{{^task}}TASK_ID_NONE{{/task}},
        {{#sig_set}}{{.}}{{/sig_set}}{{^sig_set}}0{{/sig_set}}
    },
{{/timers}}
};

/*| function_like_macros |*/
#define timer_expired(timer, timeout) ((timer)->enabled && (timer)->expiry == timeout)
#define timer_is_periodic(timer) (timer->reload > 0)
#define current_timeout() ((TicksTimeout) {{prefix}}timer_current_ticks)
#define TIMER_PTR(timer_id) (&timers[timer_id])

/*| functions |*/
static void
timer_process_one(struct timer *const timer)
{
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
        if (_signal_pending(timer->task_id, timer->signal_set))
        {
            timer->overflow = true;
        }
        {{prefix}}signal_send_set(timer->task_id, timer->signal_set);
    }
}

static void
timer_process(void)
{
    TimerId timer_id;
    struct timer *timer;
    TicksTimeout timeout;

    {{prefix}}timer_current_ticks++;
    timeout = current_timeout();

    for (timer_id = TIMER_ID_ZERO; timer_id <= TIMER_ID_MAX; timer_id++)
    {
        timer = TIMER_PTR(timer_id);
        if (timer_expired(timer, timeout))
        {
            timer_process_one(timer);
       }
   }
}

/*| public_functions |*/
void
{{prefix}}timer_enable(const TimerId timer_id)
{
    timers[timer_id].expiry = current_timeout() + timers[timer_id].reload;
    timers[timer_id].enabled = true;
}

void
{{prefix}}timer_disable(const TimerId timer_id)
{
    timers[timer_id].enabled = false;
}

void
{{prefix}}timer_oneshot(const TimerId timer_id, const TicksRelative timeout)
{
    {{prefix}}timer_reload_set(timer_id, timeout);
    {{prefix}}timer_enable(timer_id);
    {{prefix}}timer_reload_clear(timer_id);
}

bool
{{prefix}}timer_check_overflow(const TimerId timer_id)
{
    bool r = timers[timer_id].overflow;
    timers[timer_id].overflow = false;
    return r;
}

TicksRelative
{{prefix}}timer_remaining(const TimerId timer_id)
{
    return timers[timer_id].enabled ? timers[timer_id].expiry - current_timeout() : 0;
}

/* Configuration functions */
void
{{prefix}}timer_reload_set(const TimerId timer_id, const TicksRelative reload)
{
    timers[timer_id].reload = reload;
}

void
{{prefix}}timer_reload_clear(const TimerId timer_id)
{
    timers[timer_id].reload = 0;
}

void
{{prefix}}timer_signal_set(const TimerId timer_id, const TaskId task_id, const SignalSet signal_set)
{
    timers[timer_id].error_id = ERROR_ID_NONE;
    timers[timer_id].task_id = task_id;
    timers[timer_id].signal_set = signal_set;
}

void
{{prefix}}timer_error_set(const TimerId timer_id, const ErrorId error_id)
{
    timers[timer_id].error_id = error_id;
}
