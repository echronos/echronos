/*| provides |*/
blocking-mutex
mutex

/*| requires |*/
task
preempt
reentrant

/*| doc_header |*/

/*| doc_concepts |*/
## Mutexes

Mutexes provide a mechanism for controlling mutual exclusion among tasks.
Mutual exclusion can be used, for example, to ensure that only a single task operates on a data structure, device, or some other shared resource at any point in time.

Assume, for example, a system with a hardware block for CRC calculation.
Such a CRC engine might have two registers, `input`, and `result`, which are used to sequentially feed data to it and to retrieve the resulting CRC, respectively.
It could be represented by the following type and object for memory-mapped hardware access:

<pre>struct crc_engine {
    unsigned char input;
    unsigned char result;
};

struct crc_engine crc;</pre>

Its functionality could be made available to tasks by the following function:

<pre>unsigned char
crc_calculate(const unsigned char *const src,
              const unsigned char length) {
    unsigned char idx;
    for (idx = 0; idx != length; idx += 1) {
        crc.input = src[idx];
    }
    return crc.result;
}</pre>

[[^preemptive]]
It is safe for two (or more) tasks to call `crc_calculate()`.
The function does not cause any task switch, so it is guaranteed that a task enters and leaves the function without another task being scheduled in between.
Therefore, the CRC engine is only used by a single task at a time, calculating the result only from the input provided by that task.

However, if there is an explicit context switch involved, one task might start using the CRC engine while another, interrupted task has not yet completed its own use of it.
This would lead to an overlap of input values and therefore incorrect CRC results.

A typical example is a long-running iteration that yields (or causes a context switch through any other means):

<pre>[...]
for (idx = 0; idx != length; idx += 1) {
    crc.input = src[idx];
    yield();
}
[...]</pre>
[[/preemptive]]

[[#preemptive]]
However, since the RTOS is preemptive, a task switch can occur at any time inside the function.
Therefore, one task might start using the CRC engine while another, interrupted task has not yet completed its own use of it.
This would lead to an overlap of input values and therefore incorrect CRC results.
[[/preemptive]]

Mutexes can help to prevent such consistency issues.
Used correctly, a mutex ensures that only a single task at a time can execute code paths like the above.

At any given time, a mutex is in exactly one of two possible states: available or acquired.
Initially, each mutex is available.
Only after a task A acquires it via the [<span class="api">mutex_lock</span>] or [<span class="api">mutex_try_lock</span>] APIs, the mutex is in the acquired state.
After task A has finished manipulating the shared resources, it uses the [<span class="api">mutex_unlock</span>] API to put the mutex back into the available state.
This allows other tasks to acquire it:

<pre>unsigned char
crc_calculate(const unsigned char *const src,
              const unsigned char length) {
    unsigned char idx, result;
    mutex_lock(RTOS_MUTEX_ID_CRC);
    for (idx = 0; idx != length; idx += 1) {
        crc.input = src[idx];
[[^preemptive]]
        yield();
[[/preemptive]]
    }
    result = crc.result;
    mutex_unlock(RTOS_MUTEX_ID_CRC);
    return result;
}</pre>

The API guarantees that only a single task in the system can hold a mutex at a given time.
In other words, it is guaranteed that when any number of tasks attempt to acquire a mutex, only one of them succeeds.
When a task A has already acquired a mutex and another task B uses [<span class="api">mutex_lock</span>] on the same mutex, the API blocks task B.
Only when task A releases the mutex, task B unblocks, acquires the mutex, and returns from its call to [<span class="api">mutex_lock</span>].

It is an implementation error for a task to call [<span class="api">mutex_lock</span>] on a mutex it has already acquired.
It is also an implementation error to call [<span class="api">mutex_unlock</span>] on a mutex not acquired by the calling task (i.e., if the mutex is either not acquired by any task or acquired by a different task).

The [<span class="api">mutex_try_lock</span>] API allows a task to avoid being blocked by always returning immediately.
If the mutex is already acquired by another task, the API returns immediately, indicating that the calling task has not acquired the mutex.

The system designer defines at configuration time which mutexes are available in a system.
The main property of a mutex is its name, such as `CRC` in the example above.
Therefore, the implementation refers to a mutex via its symbolic name [`MUTEX_ID_<name>`] that has the type [<span class="api">MutexId</span>].
Applications must not make any assumptions about or rely on the numeric values that the configuration tool assigns to such symbolic names.

As with any other objects that are not interrupt events, mutexes and their related APIs can not be used by interrupt service routines.

/*| doc_api |*/
## Mutex API

### <span class="api">MutexId</span>

An instance of this type refers to a specific mutex.
The underlying type is an unsigned integer of a size large enough to represent all mutexes[^MutexId_width].

[^MutexId_width]: This is normally a `uint8_t`.

### `MUTEX_ID_<name>`

These constants of type [<span class="api">MutexId</span>] exist for all mutexes defined in the system configuration.
`<name>` is the upper-case conversion of the mutex's name.

Applications shall use the symbolic names [`MUTEX_ID_<name>`] to refer to mutexes wherever possible.
Applications shall not rely on the numeric value of a mutex ID.
Across two different RTOS and applications builds, the ID for the same mutex may have different numeric values.

### `MUTEX_ID_ZERO` and `MUTEX_ID_MAX`

The IDs of all mutexes are guaranteed to be a contiguous integer range between `MUTEX_ID_ZERO` and `MUTEX_ID_MAX`, inclusive.
Applications may iterate over all mutexes via `for (id = MUTEX_ID_ZERO; id <= MUTEX_ID_MAX; id += 1)`.
Also, they may associate information with mutexes, for example as follows:

<pre>unsigned int mutex_lock_count[1 + MUTEX_ID_MAX - MUTEX_ID_ZERO];
#define mutex_lock_and_count(ID)\
do {\
    mutex_lock(ID);\
    mutex_lock_count[(ID) - MUTEX_ID_ZERO] += 1;\
} while (0)</pre>



### <span class="api">mutex_lock</span>

<div class="codebox">void mutex_lock(MutexId mutex);</div>

This API acquires the specified mutex.
It returns only after it has acquired the mutex, which may be either immediately or after another task has released the mutex.
A task must release a mutex before acquiring it again.

If the mutex is in the *available* state, it transitions into the *acquired* state and the API returns immediately.

If the mutex is in the *acquired* state, the mutex state does not change, but the calling task blocks and a task switch occurs.
After the task unblocks and becomes the current task again, it attempts to acquire the mutex again in the same fashion until successful.

This API is guaranteed to return only after the calling task has transitioned successfully the mutex from the *available* into the *acquired* state.

This implies that a task cannot successfully acquire the same mutex twice without releasing it in between.
Attempting to do so effectively blocks the calling task indefinitely.

[[#lock_timeout]]

### <span class="api">mutex_lock_timeout</span>

<div class="codebox">bool mutex_lock_timeout(MutexId mutex, TicksRelative timeout);</div>

This function waits a maximum *timeout* number of ticks to acquire the specified mutex.

Its behaviour matches that of [<span class="api">mutex_lock</span>], except that the maximum amount of time that the calling task can be blocked is bounded by the *timeout* number of ticks given.
For more information, see [Time and Timers].

If <span class="api">mutex_lock_timeout</span> successfully acquires the mutex, it returns true.
Otherwise, it returns false.

The system designer must not use this function to attempt to acquire a mutex previously acquired by the same task without releasing it in between.

[[/lock_timeout]]

### <span class="api">mutex_try_lock</span>

<div class="codebox">bool mutex_try_lock(MutexId mutex);</div>

This API attempts to acquire the specified mutex.
It returns true if it successfully acquired the mutex and false if the mutex is already acquired by another task.

If the mutex is in the *available* state, it transitions into the *acquired* state and the API returns true immediately.

If the mutex is in the *acquired* state, the mutex state does not change and the API returns false immediately.

This API does not cause a task switch.


### <span class="api">mutex_unlock</span>

<div class="codebox">void mutex_unlock(MutexId mutex);</div>

This API releases the specified mutex so that other tasks can acquire it.
A task should not release a mutex that it has not previously acquired.

This API transitions a mutex into the *available* state, unblocks all blocked tasks that have called [<span class="api">mutex_lock</span>] while it was in the *acquired* state, and returns.

[[^preemptive]]
This API does not cause a task switch.
[[/preemptive]]
[[#preemptive]]
This API may cause a task switch.
[[/preemptive]]


### <span class="api">mutex_holder_is_current</span>

<div class="codebox">bool mutex_holder_is_current(MutexId mutex);</div>

This API returns whether the current task is the holder of the specified mutex at the time the API is called.


/*| doc_configuration |*/
## Mutex Configuration

### `mutexes`

The `mutexes` configuration is a list of `mutex` configuration objects.
See the [Mutex Configuration] section for details on configuring each mutex.

### `mutexes/mutex/name`

This configuration item specifies the mutex's name.
Each mutex must have a unique name.
The name must be of an identifier type.
This is a mandatory configuration item with no default.

/*| doc_footer |*/
