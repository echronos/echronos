/*| dependencies |*/
error
signal
task

/*| doc_concepts |*/
## Time and Timers

It is not surprising that time is an important aspect of most real-time systems.
The RTOS provides a number of important interfaces for keeping track of time.
The RTOS tracks overall execution of the system, and additionally provides timer objects that allows a task to schedule a certain action to happen after a specific duration of time.

All the time-based interfaces in the RTOS are based around a tick concept.
The RTOS depends on the system to provide a suitable timer driver and inform the RTOS on each tick.
The system designer should ensure that the RTOS [<span class="api">timer_tick</span>] API is called periodically.
The RTOS does not define what the tick period should be;
this is a decision for the system designer.

The [<span class="api">timer_tick</span>] API is safe to call from an interrupt service routine.
The tick is processed the when the current task yields or blocks.
When a timer tick is processed the [<span class="api">timer_current_ticks</span>] is incremented and any enabled timer objects are decremented[^timer_decrement].

[^timer_decrement]: The actual implementation does not decrement each timer, however this is an accurate description of the logical behaviour.

When a timer is decremented to zero the timer expires.
The exact behaviour that occurs when a timer expires depends on the configuration of the timer object.

A timer object may be configured as a watchdog.
When a watchdog timer expires it causes a fatal error to occur (see [Error Handling]).

Alternatively, when a timer expires it sends a configurable set of signals to a specific task.
If the timer is periodic, it automatically starts counting down again.
Alternatively a timer may be a one-shot timer, in which case it becomes disabled.

As timers are based on ticks it is important to understand some of the limitations that this imposes.
Firstly, the best possible timing resolution is limited by the tick period.
If we assume a 40Hz tick (25ms period), then a desired period of 30ms must either be rounded down to 25ms or up to 50ms.

The second limiting factor is that ticks are only processed when a task yields or block, so the processing of a tick can be delayed for some time.
To ensure that this delay is not indefinite the RTOS requires tasks to avoid running for longer than a tick period.
For long running tasks this means that the task must yield at a higher frequency than the tick.
With this restriction in place the delay for processing a tick is at most a tick period.
From a practical point of view this means that if the [<span class="api">timer_current_ticks</span>] variable reads as 10 the actual elapsed time could be anywhere 250ms and 300ms (more generally elapsed time is in the range `timer_current_ticks * tick_period to (timer_current_ticks + 2) * tick_period`).
To see how this can happen, consider the case where [<span class="api">timer_current_ticks</span>] is 10.
If a task is scheduled and then the next tick occurs (which indicates 275ms of real-time has elapsed), the tick is not processed until the newly scheduled task yields (or blocks).
Since the task can execute for up to 25ms, it is possible for the elapsed time to reach (just under) 300ms, with the [<span class="api">timer_current_ticks</span>] still reading as 10 (250ms).
Another consequence of this is that actual length of the time between two ticks being processed can be anywhere from zero to 2 times the tick period (e.g.: 0ms to 50ms in the current example).
If we continue the previous example and the task blocks at after executing for about 25ms, then the pending tick (which is now just under 25ms overdue) is processed.
Immediately after the pending tick is processed, the new tick happens, and may be processed immediately (if, for example, the RTOS is idle).

It is useful to consider the impact on actual timer objects.
First, consider a periodic timer, which is set up with a period of 10 ticks.
Using the same 25ms tick period the time between consecutive timer expiries can range between 225ms and 275ms.
Over long periods of time, the average time between consecutive expiries approaches the nominal 250ms value.
It is important that a programmer take this into consideration when designing software.
A similar level of impression is evident in a one-shot timer.
A 10 tick one-shot timer expires between 225ms and 275ms after it is set.
If the requirement a programmer was trying to reach was to wait for at least 250ms, then a setting of 11-ticks should be used.

Note that the above latency considerations also apply to the value of the [<span class="api">timer_current_ticks</span>] API.

/*| doc_api |*/
## Timers

### <span class="api">TimerId</span>

A [<span class="api">TimerId</span>] refers to a specific timer.
The underlying type is an unsigned integer of a size large enough to represent all timers[^TimerId_width].
The [<span class="api">TimerId</span>] should be treated an an opaque value.
For all timers in the system the configuration tool creates a constant with the name `TIMER_ID_<name>` that should be used in preference to raw values.

[^TimerId_width]: This is normally a `uint8_t`.

### <span class="api">TicksAbsolute</span>

The [<span class="api">TicksAbsolute</span>] type is used to represents an absolute number of ticks.
It is a 32-bit unsigned integer that has a large enough range to represent the total number of ticks since system boot.
For a 40Hz (25ms period) tick this can handle over 1,200 days worth of ticks.

### <span class="api">TicksRelative</span>

The [<span class="api">TicksRelative</span>] type is used to represents a number of ticks relative to a point in time.
It is a 16-bit unsigned integer.
Assuming a 40Hz tick this provides range for up to 27 minutes worth of ticks.

### `TIMER_ID_<name>`

`TIMER_ID_<name>` has the type [<span class="api">TimerId</span>].
A constant is created for each timer in the system.
Note that *name* is the upper-case conversion of the timer's name.

### <span class="api">timer_current_ticks</span>

<div class="codebox">TicksAbsolute timer_current_ticks;</div>

The value of this variable is the current global tick count in the system.
It directly reflects how many time the <span class="api">timer_tick</span> API has been called since the system startup.

### <span class="api">timer_tick</span>

<div class="codebox">void timer_tick(void);</div>

The [<span class="api">timer_tick</span>] API is to be called by an interrupt service routine to register a system tick with the RTOS.
The RTOS timer functionality is based on a system timer periodically calling this function.
The registered tick remains pending until the RTOS processes the tick.
This and `interrupt_event_raise_<name>` are the only RTOS API functions that an interrupt service routine may call.

### <span class="api">timer_enable</span>

<div class="codebox">void timer_enable(TimerId timer);</div>

The [<span class="api">timer_enable</span>] API is called to start a timer.
The timer starts counting down from its configured reload value.
If the timer's reload value is zero, then the timer expires immediately (and perform its configured expiry behaviour).

### <span class="api">timer_disable</span>

<div class="codebox">void timer_disable(TimerId timer);</div>

The [<span class="api">timer_disable</span>] API stops a timer.
The configured expiry action does not occur if the timer is disabled before it has expired.
Disabling a timer does not simply pause the timer;
if the timer is reenabled (with the [<span class="api">timer_enable</span>] API), it starts counting from the configured reload value.

### <span class="api">timer_oneshot</span>

<div class="codebox">void timer_oneshot(TimerId timer, TicksRelative timeout);</div>

The [<span class="api">timer_oneshot</span>] API starts a timer in a one-shot manner.
When the timer expires, it does not automatically restart.
The timer expires after `timeout` number of ticks.

### <span class="api">timer_check_overflow</span>

<div class="codebox">bool timer_check_overflow(TimerId timer);</div>

The [<span class="api">timer_check_overflow</span>] API returns true if a timer has overflowed.
A timer overflows if the signal it sends on expiry would be lost.
For example, if a task is expecting to receive signals from a periodic timer and does not receive them quickly enough, the timer is marked as having overflowed.
Calling the [<span class="api">timer_check_overflow</span>] API clears the overflow mark if set.

### <span class="api">timer_remaining</span>

<div class="codebox">TicksRelative timer_remaining(TimerId timer);</div>

The [<span class="api">timer_remaining</span>] API returns the number of ticks remaining before the specified timer expires.

### <span class="api">timer_reload_set</span>

<div class="codebox">void timer_reload_set(TimerId timer, TicksRelative reload_value);</div>

The [<span class="api">timer_reload_set</span>] API configures a timer's reload value.
The `reload` value is use to initialise the timer when it is enabled (or when it restarts in the case of a periodic timer).

### <span class="api">timer_signal_set</span>

<div class="codebox">void timer_signal_set(TimerId timer, `TaskId` task, SignalSet sigset);</div>

The [<span class="api">timer_signal_set</span>] API configures the timer so that on expiry it sends the specified sigset to the specified task.

### <span class="api">timer_error_set</span>

<div class="codebox">void timer_error_set(TimerId timer, ErrorCode error);</div>

The [<span class="api">timer_signal_set</span>] API configures the timer so that on expiry it causes a fatal error to occur.
The specified `error` code is passed to the configured `fatal_error` function.

/*| doc_configuration |*/
## Timer Configuration

### `timers`

The `timers` configuration is a list of timer configuration objects.
See the [Timer Configuration] section for details on configuring each task.

### `name`

This configuration item specifies the timer's name.
Each timer must have a unique name.
The name must be of an identifier type.
This is a mandatory configuration item with no default.

### `enabled`

This boolean configuration item determines if the timer is enabled at startup.
If a timer is enabled at startup, it commences countdown on the very first tick.
This is an optional configuration item that defaults to false.
A timer can be enabled at runtime using the [<span class="api">timer_enable</span>] API.

### `reload`

This configuration item specifies the timer's reload value.
The reload value is used to initialise the timer each time it is enabled.
The value is specified as a relative number of ticks.
The value must be presentable as a [<span class="api">TicksRelative</span>] type.
This is an optional configuration item that defaults to zero.

### `task`

This configuration item specifies the task to which a signal set is sent when the timer expires.
This configuration item is optional.
If no task is set when the timer expires, a fatal error occurs.
If the system designer does not set the task in the static configuration, it can be set at runtime using the [<span class="api">timer_signal_set</span>] API.

### `sig_set`

This configuration item specifies the signal set that is sent to the timer's associated task.
A signal set is a list of one or more specified signal labels.
This configuration item is optional and defaults to the empty set.

### `error`

This configuration item specifies the error code that is passed to the `fatal_error` function when the timer expires.
This configuration item is optional and defaults to zero.
This should not be set if a task is specified.

/*| schema |*/
<entry name="timers" type="list" default="[]" auto_index_field="idx">
    <entry name="timer" type="dict">
        <entry name="name" type="ident" />
        <entry name="enabled" type="bool" />
        <entry name="reload" type="int" />
        <entry name="error" type="int" default="0" />
        <entry name="task" type="object" group="tasks" optional="true" />
        <entry name="sig_set" type="ident" optional="true" />
    </entry>
</entry>

/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t {{prefix_type}}TimerId;
typedef uint32_t {{prefix_type}}TicksAbsolute;
typedef uint16_t {{prefix_type}}TicksRelative;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#timers}}
#define {{prefix_const}}TIMER_ID_{{name|u}} (({{prefix_type}}TimerId) UINT8_C({{idx}}))
{{/timers}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/
extern {{prefix_type}}TicksAbsolute {{prefix_func}}timer_current_ticks;

/*| public_function_definitions |*/
{{#timers.length}}
void {{prefix_func}}timer_enable({{prefix_type}}TimerId timer_id);
void {{prefix_func}}timer_disable({{prefix_type}}TimerId timer_id);
void {{prefix_func}}timer_oneshot({{prefix_type}}TimerId timer_id, {{prefix_type}}TicksRelative timeout);
bool {{prefix_func}}timer_check_overflow({{prefix_type}}TimerId timer_id);
{{prefix_type}}TicksRelative {{prefix_func}}timer_remaining({{prefix_type}}TimerId timer_id);
void {{prefix_func}}timer_reload_set({{prefix_type}}TimerId timer_id, {{prefix_type}}TicksRelative reload);
void {{prefix_func}}timer_error_set({{prefix_type}}TimerId timer_id, {{prefix_type}}ErrorId error_id);
void {{prefix_func}}timer_signal_set({{prefix_type}}TimerId timer_id, {{prefix_type}}TaskId task_id, {{prefix_type}}SignalSet signal_set);
{{/timers.length}}

/*| headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| object_like_macros |*/
{{#timers.length}}
#define TIMER_ID_ZERO (({{prefix_type}}TimerId) UINT8_C(0))
#define TIMER_ID_MAX (({{prefix_type}}TimerId) UINT8_C({{timers.length}} - 1U))
{{/timers.length}}

/*| type_definitions |*/
typedef uint16_t TicksTimeout;

/*| structure_definitions |*/
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

/*| extern_definitions |*/

/*| function_definitions |*/

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
#define current_timeout() ((TicksTimeout) {{prefix_func}}timer_current_ticks)
#define TIMER_PTR(timer_id) (&timers[timer_id])
{{/timers.length}}
#define assert_timer_valid(timer) api_assert(timer_id < {{timers.length}}, ERROR_ID_INVALID_ID)


/*| functions |*/
{{#timers.length}}
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
        {{prefix_func}}signal_send_set(timer->task_id, timer->signal_set);
    }
}
{{/timers.length}}

static void
timer_tick_process(void)
{
    const uint8_t pending_ticks = timer_pending_ticks_get_and_clear_atomically();

    if (pending_ticks > 1)
    {
        {{fatal_error}}(ERROR_ID_TICK_OVERFLOW);
    }

    if (pending_ticks)
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

/*| public_functions |*/
{{#timers.length}}
void
{{prefix_func}}timer_enable(const {{prefix_type}}TimerId timer_id)
{
    assert_timer_valid(timer_id);

    if (timers[timer_id].reload == 0)
    {
        timer_process_one(&timers[timer_id]);
    }
    else
    {
        timers[timer_id].expiry = current_timeout() + timers[timer_id].reload;
        timers[timer_id].enabled = true;
    }
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

    {{prefix_func}}timer_reload_set(timer_id, timeout);
    {{prefix_func}}timer_enable(timer_id);
    {{prefix_func}}timer_reload_set(timer_id, 0);
}

bool
{{prefix_func}}timer_check_overflow(const {{prefix_type}}TimerId timer_id)
{
    bool r;

    assert_timer_valid(timer_id);

    r = timers[timer_id].overflow;
    timers[timer_id].overflow = false;
    return r;
}

{{prefix_type}}TicksRelative
{{prefix_func}}timer_remaining(const {{prefix_type}}TimerId timer_id)
{
    assert_timer_valid(timer_id);

    return timers[timer_id].enabled ? timers[timer_id].expiry - current_timeout() : 0;
}

/* Configuration functions */
void
{{prefix_func}}timer_reload_set(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}TicksRelative reload)
{
    assert_timer_valid(timer_id);

    timers[timer_id].reload = reload;
}

void
{{prefix_func}}timer_signal_set(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}TaskId task_id, const {{prefix_type}}SignalSet signal_set)
{
    assert_timer_valid(timer_id);

    timers[timer_id].error_id = ERROR_ID_NONE;
    timers[timer_id].task_id = task_id;
    timers[timer_id].signal_set = signal_set;
}

void
{{prefix_func}}timer_error_set(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}ErrorId error_id)
{
    assert_timer_valid(timer_id);

    timers[timer_id].error_id = error_id;
}
{{/timers.length}}
