/*| provides |*/
timer

/*| requires |*/
task
signal
error
interrupt-event-signal

/*| doc_header |*/

/*| doc_concepts |*/
## Time and Timers

It is not surprising that time is an important aspect of most real-time systems.
The RTOS provides a number of important interfaces for keeping track of time.
The RTOS tracks overall execution of the system, and additionally provides timer objects that allow tasks to schedule certain actions to happen after a specific duration of time.

### Ticks

All of the time-based interfaces in the RTOS are based around a tick concept.
The RTOS does not keep track of physical or wall-clock time;
instead it counts only ticks, so time and durations are expressed only in ticks in the RTOS API.
How much absolute physical time a tick represents is left to the system design.

The RTOS depends on the system to generate ticks, provide a suitable system tick driver, and inform the RTOS of each tick.
The system designer should ensure that the RTOS [<span class="api">timer_tick</span>] API is called for each tick.
The [<span class="api">timer_tick</span>] API is safe to call from an interrupt service routine.
[[^preemptive]]
A tick is not processed immediately but when the current task yields or blocks.
[[/preemptive]]

When a system tick is processed, the [<span class="api">timer_current_ticks</span>] API value is incremented by one.
This allows tasks to keep track of the amount of time that the system has been running for, or shorter durations as they choose.

### Timers

Timers trigger certain actions, such as sending a signal, at a configurable time (expressed in ticks).
The [Timer Configuration] defines which timers exist in a system and what their properties are.

The main property of a timer is its tick value, which says how many ticks in the future the timer will expire.
When a system tick is processed, any enabled timer objects are decremented[^timer_decrement] by one.
When a timer is decremented to zero, the timer is said to expire.
The exact behavior that occurs when a timer expires depends on the configuration of the timer object.

[^timer_decrement]: The actual implementation does not decrement each timer, although it is an accurate description of the logical behavior.

- A timer object may be configured as a watchdog.
When a watchdog timer expires, it causes a fatal error to occur (see [Error Handling]).

- Alternatively, a timer may be configured to send, when it expires, a configurable set of signals to a configurable task.
If the timer is periodic, it automatically starts counting down again.

- Alternatively, a timer may be a one-shot timer, in which case it is disabled upon expiring, so it does not start counting down again.

### Timing Considerations

As timers are based on ticks, it is important to understand some of the limitations that this imposes.

Firstly, the best possible timing resolution is limited by the tick period.
If we assume a 40Hz tick (25ms period), then a desired period of 30ms must either be rounded down to 25ms or up to 50ms.

[[^preemptive]]
The second limiting factor comes into play because the RTOS is non-preemptive.
Ticks are processed only when a task yields or blocks, so there is a delay between the system calling the [<span class="api">timer_tick</span>] API and the RTOS processing the tick, including timer handling.
To ensure that this delay is bounded, the RTOS requires tasks to not run for longer than a tick period.
For long running tasks this means that the task must yield at a higher frequency than the tick.
With this restriction in place, the delay for processing a tick is at most a tick period.
[[/preemptive]]

From a practical point of view this means that if the [<span class="api">timer_current_ticks</span>] variable reads as 10, the actual elapsed time could be anywhere between 250ms and 300ms (more generally elapsed time is in the range `timer_current_ticks * tick_period` to `(timer_current_ticks + 2) * tick_period`).
To see how this can happen, consider the case where [<span class="api">timer_current_ticks</span>] is 10.
If a task is scheduled and then the next tick occurs (which indicates 275ms of real-time has elapsed), the tick is not processed until the newly scheduled task yields (or blocks).
Since the task can execute for up to 25ms, it is possible for the elapsed time to reach (just under) 300ms, with the [<span class="api">timer_current_ticks</span>] still reading as 10 (250ms).
Another consequence of this is that actual length of the time between two ticks being processed can be anywhere from zero to 2 times the tick period (e.g.: 0ms to 50ms in the current example).
If we continue the previous example and the task blocks at after executing for about 25ms, then the pending tick (which is now just under 25ms overdue) is processed.
Immediately after the pending tick is processed, the new tick happens, and may be processed immediately (if, for example, the RTOS is idle).

It is useful to consider the impact on actual timer objects.
First, assume a periodic timer, which is set up with a period of 10 ticks.
Using the same 25ms tick period as above, the time between consecutive timer expiries can range between 225ms and 275ms.
Only over long periods of time, the average time between consecutive expiries approaches the nominal 250ms value.
A similar level of imprecision is evident in a one-shot timer.
A 10-tick one-shot timer expires between 225ms and 275ms after it is set.
When a timer is required to expire after at least 250ms but not before then, it needs to be set to 11 ticks.

Note that the above latency considerations also apply to the value of the [<span class="api">timer_current_ticks</span>] and [<span class="api">sleep</span>] APIs.

/*| doc_api |*/
## Sleep API

### <span class="api">sleep</span>

<div class="codebox">void sleep(TicksRelative ticks);</div>

The [<span class="api">sleep</span>] API blocks the current task for the specified number of timer ticks.
After `ticks` timer ticks, the task becomes runnable again.
Note that that does not immediately make it the current task again.
See the [Time and Timers] section for further information on timing and scheduling considerations.

## Timer API

### <span class="api">TimerId</span>

A [<span class="api">TimerId</span>] refers to a specific timer.
The underlying type is an unsigned integer of a size large enough to represent all timers[^TimerId_width].
The [<span class="api">TimerId</span>] value should be treated as an opaque value.
For all timers in the system, the configuration tool creates a constant with the name `TIMER_ID_<name>` that should be used in preference to raw values.

[^TimerId_width]: This is normally a `uint8_t`.

### <span class="api">TicksAbsolute</span>

The [<span class="api">TicksAbsolute</span>] type is used to represent an absolute number of ticks.
It is a 32-bit unsigned integer that has a large enough range to represent the total number of ticks since system boot.
For a 40Hz (25ms period) tick, this can handle over 1,200 days worth of ticks.

### <span class="api">TicksRelative</span>

The [<span class="api">TicksRelative</span>] type is used to represent a number of ticks relative to a point in time.
It is a 16-bit unsigned integer.
Assuming a 40Hz tick this provides range for up to 27 minutes' worth of ticks.

### `TIMER_ID_<name>`

`TIMER_ID_<name>` has the type [<span class="api">TimerId</span>].
A constant is created for each timer in the system.
Note that *name* is the upper-case conversion of the timer's name.

### <span class="api">timer_current_ticks</span>

<div class="codebox">TicksAbsolute timer_current_ticks;</div>

The value of this variable is the current global tick count in the system.
It directly reflects how many times the <span class="api">timer_tick</span> API has been called since the system startup.

### <span class="api">timer_tick</span>

<div class="codebox">void timer_tick(void);</div>

This API is to be called to register a system tick with the RTOS.
It is usually triggered by a periodic, external event, such as an interrupt generated by a hardware timer.
It is safe to call this API directly from an interrupt service routine.

Note that the RTOS timer functionality directly depends on this API being called regularly.
The registered tick remains pending until the RTOS processes the tick (see [Timing Considerations]).

### <span class="api">timer_enable</span>

<div class="codebox">void timer_enable(TimerId timer);</div>

This API is called to start a timer.
The timer starts counting down from its configured reload value.
If the timer's reload value is zero, then the timer expires immediately (and performs its configured expiry behavior).

### <span class="api">timer_disable</span>

<div class="codebox">void timer_disable(TimerId timer);</div>

This API stops a timer.
The configured expiry action does not occur if the timer is disabled before it has expired.
Disabling a timer does not simply pause the timer;
if the timer is reenabled (with the [<span class="api">timer_enable</span>] API), it starts counting from the configured reload value.

### <span class="api">timer_oneshot</span>

<div class="codebox">void timer_oneshot(TimerId timer, TicksRelative timeout);</div>

This API starts a timer in a one-shot manner.
When the timer expires, it does not automatically restart.
The timer expires after `timeout` number of ticks.

### <span class="api">timer_check_overflow</span>

<div class="codebox">bool timer_check_overflow(TimerId timer);</div>

This API returns true if a timer has overflowed.
A timer overflows if the signal it sends on expiry would be lost.
For example, if a task is expecting to receive signals from a periodic timer and does not receive them quickly enough, the timer is marked as having overflowed.
Calling the [<span class="api">timer_check_overflow</span>] API clears the overflow mark if set.

[[#preemptive]]
The calling task may be subject to an unpredictable amount of delay between calling this function and evaluating its return value, in the case that the task is preempted.
[[/preemptive]]

### <span class="api">timer_remaining</span>

<div class="codebox">TicksRelative timer_remaining(TimerId timer);</div>

This API returns the number of ticks remaining before the specified timer expires.

[[#preemptive]]
The calling task may be subject to an unpredictable amount of delay between calling this function and evaluating its return value, in the case that the task is preempted.
[[/preemptive]]

### <span class="api">timer_reload_set</span>

<div class="codebox">void timer_reload_set(TimerId timer, TicksRelative reload_value);</div>

This API configures a timer's reload value.
The `reload` value is used to initialize the timer when it is enabled (or when it restarts in the case of a periodic timer).

### <span class="api">timer_signal_set</span>

<div class="codebox">void timer_signal_set(TimerId timer, TaskId task, SignalSet sigset);</div>

This API configures the timer so that on expiry it sends the set of signals `sigset` to the specified task.

### <span class="api">timer_error_set</span>

<div class="codebox">void timer_error_set(TimerId timer, ErrorId error);</div>

This API configures the timer so that on expiry it causes a fatal error to occur.
The specified `error` code is passed to the configured [`fatal_error`] function.

/*| doc_configuration |*/
## Timer Configuration

### `timers`

The `timers` configuration is a list of `timer` configuration objects.

### `timers/timer/name`

This configuration item specifies the timer's name.
Each timer must have a unique name.
The name must be of an identifier type.
This is a mandatory configuration item with no default.

### `timers/timer/enabled`

This boolean configuration item determines whether the timer is enabled when the system starts.
If it is true, the timer commences countdown on the very first tick.
This is an optional configuration item that defaults to false.
A timer can be enabled at runtime using the [<span class="api">timer_enable</span>] API.

### `timers/timer/reload`

This configuration item specifies the timer's reload value.
The reload value is used to initialize the timer each time it is enabled.
The value is specified as a relative number of ticks.
The value must be presentable as a [<span class="api">TicksRelative</span>] type.
This is an optional configuration item that defaults to zero.

### `timers/timer/task`

This configuration item specifies the task to which a signal set is sent when the timer expires.
This configuration item is optional.
If no task is set when the timer expires, a fatal error occurs.
If the system designer does not set the task in the static configuration, it can be set at runtime using the [<span class="api">timer_signal_set</span>] API.

### `timers/timer/sig_set`

This configuration item specifies the signal set that is sent to the timer's associated task.
A signal set is a list of one or more specified signal labels.
This configuration item is optional and defaults to the empty set.

### `timers/timer/error`

This configuration item specifies the error code that is passed to the `fatal_error` function when the timer expires.
This configuration item is optional and defaults to zero.
This should not be set if a task is specified.

/*| doc_footer |*/
