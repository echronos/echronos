/*| provides |*/
simple-semaphore
semaphore

/*| requires |*/
reentrant
task
preempt
sched

/*| doc_header |*/

/*| doc_concepts |*/
## Semaphores

Semaphores provide a signaling mechanism.
Conceptually, a semaphore is an integral value with two operations: *post* and *wait*.
Post is sometimes called *V*, *signal*, or *up*.
Wait is sometimes called *P* or *down*.
The post operation increments the underlying value, whereas the wait operation decrements the underlying value, if (and only if) the current value of the semaphore is greater than zero.
The wait operation blocks the calling task until the semaphore value can be successfully decremented.

Unlike [Mutexes], a semaphore has no concept of a task that holds the semaphore.
On RTOS variants using priority-based scheduling with priority inheritance, a consequence of this is that when a task waits on a semaphore, no task will inherit the waiting task's priority.
Please see [Scheduling Algorithm] for more details on the scheduling algorithm in use by this variant.

The post operation is made available through the [<span class="api">sem_post</span>] API.
The wait operation is made available through the [<span class="api">sem_wait</span>] API.
Additionally, the [<span class="api">sem_try_wait</span>] API allows a task to attempt to decrement a semaphore without blocking.

/*| doc_api |*/
## Semaphores

### <span class="api">SemId</span>

Instances of this type refer to specific semaphores.
The type is an unsigned integer of a size large enough to represent all semaphores[^SemId_width].

[^SemId_width]: This is normally a `uint8_t`.

### `SEM_ID_<name>`

These constants of type [<span class="api">SemId</span>] exist for all semaphores defined in the system configuration.
`<name>` is the upper-case conversion of the semaphore's name.

Applications shall use the symbolic names [`SEM_ID_<name>`] to refer to semaphores wherever possible.
Applications shall not rely on the numeric value of a semaphore ID.
Across two different RTOS and applications builds, the ID for the same semaphore may have different numeric values.

### `SEM_ID_ZERO` and `SEM_ID_MAX`

The IDs of all semaphores are guaranteed to be a contiguous integer range between `SEM_ID_ZERO` and `SEM_ID_MAX`, inclusive.
Applications may iterate over all semaphores via `for (id = SEM_ID_ZERO; id <= SEM_ID_MAX; id += 1)`.

### <span class="api">SemValue</span>

This type represents a semaphore's current and, optionally, maximum value (see [<span class="api">sem_max_init</span>]).
The bit width of the <span class="api">SemValue</span> type depends on the configuration item [`semaphore_value_size`].

### <span class="api">sem_max_init</span>

<div class="codebox">void sem_max_init(SemId sem, SemValue max);</div>

This function is only available if the [`semaphore_enable_max`] configuration item is true.
It initializes the specified semaphore with the given maximum value, which must be non-zero.
The application must call it once and only once per semaphore, and must do so before using the [<span class="api">sem_post</span>] API with the semaphore.
The maximum value of a semaphore influences the behavior of the [<span class="api">sem_post</span>] API.

### <span class="api">sem_post</span>

<div class="codebox">void sem_post(SemId sem);</div>

This function increments the semaphore value by one.
Additionally, it makes all tasks runnable that have called [<span class="api">sem_wait</span>] and are currently blocked on the semaphore.

If the configuration item [`semaphore_enable_max`] is true, the following applies:

- Before an application calls <span class="api">sem_post</span> for a semaphore, it must call [<span class="api">sem_max_init</span>] once and only once for that semaphore.

- Calling <span class="api">sem_post</span> when the semaphore value is equal to the semaphore maximum is considered a fatal error.
  In that case, the RTOS implementation calls the `fatal_error` function (see [Error Handling]).

[[^preemptive]]
<span class="api">sem_post</span> does not itself cause a context switch.
The calling task remains the current task.
[[/preemptive]]
[[#preemptive]]
One of the tasks made runnable may preempt the calling task, depending on the [Scheduling Algorithm].
[[/preemptive]]


### <span class="api">sem_wait</span>

<div class="codebox">void sem_wait(SemId sem);</div>

This function waits until the semaphore value of *sem* is greater than zero and then decrements the semaphore value.

If the semaphore value of *sem* is zero, <span class="api">sem_wait</span> blocks the calling task.
When another task makes it runnable again via the [<span class="api">sem_post</span>] function, <span class="api">sem_wait</span> checks the semaphore value again in the same manner.
If the semaphore value is greater than zero, <span class="api">sem_wait</span> decrements the semaphore value by one and returns.

Thus, <span class="api">sem_wait</span> returns immediately if the semaphore value is initially greater than zero.
If the semaphore value is initially 0, however, the calling task may be blocked for an unbounded amount of time.
The semaphore implementation itself does not guarantee progress if there are multiple tasks waiting on the semaphore.
Which waiting task gets to decrement the semaphore value and return from <span class="api">sem_wait</span> depends entirely on the [Scheduling Algorithm].

[[#timeouts]]

### <span class="api">sem_wait_timeout</span>

<div class="codebox">bool sem_wait_timeout(SemId sem, TicksRelative timeout);</div>

This function waits a maximum *timeout* number of ticks until the semaphore value of *sem* is greater than zero, in which case it will decrement the semaphore value and return true.

If the *timeout* number of ticks elapses before the semaphore value can be decremented, then <span class="api">sem_wait_timeout</span> will return false.
The system designer should ensure that the RTOS [<span class="api">timer_tick</span>] API is called for each tick.
For more information, see [Time and Timers].

The behavior of <span class="api">sem_wait_timeout</span> matches that of [<span class="api">sem_wait</span>], except that the maximum amount of time that the calling task can be blocked is bounded by the *timeout* number of ticks given.

[[/timeouts]]

### <span class="api">sem_try_wait</span>

<div class="codebox">bool sem_try_wait(SemId sem);</div>

This function attempts to decrement the semaphore value of the specified semaphore without blocking the calling task.

If the semaphore value is positive, it is decremented and <span class="api">sem_try_wait</span> returns true.
Otherwise, the function returns false and the semaphore value is not modified.
This function does not cause a context switch.

/*| doc_configuration |*/
## Semaphore Configuration

### `semaphore_value_size`

This optional integer configuration item specifies the width of the [<span class="api">SemValue</span>] type in bits.
Valid values are 8, 16, and 32, with 8 being the default.

### `semaphore_enable_max`

This boolean value controls whether semaphores have a maximum value or not.
When set to true, the [<span class="api">sem_max_init</span>] function is available and the [<span class="api">sem_post</span>] function enforces the maximum value.
This is an optional configuration item that defaults to false.

### `semaphores`

This configuration item is a list of [`semaphores/semaphore`] configuration objects.

### `semaphores/semaphore`

This configuration item is a dictionary of values defining the properties of a single semaphore.

### `semaphores/semaphore/name`

This configuration item specifies the name of a semaphore.
Each semaphore must have a unique name.
The name must be of an identifier type.
This is a mandatory configuration item with no default.

/*| doc_footer |*/
