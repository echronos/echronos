/*| doc_header |*/

/*| doc_concepts |*/
## Mutexes

Mutexes provide a low-level mechanism for controlling mutual exclusion.
Mutual exclusion can be used to ensure that only a single task operates on a data structure, or device, at any point in time.
It is important however to remember that a task is not preempted by other tasks in the system, so a mutex is only required in the case where exclusive access exists across a yield or blocking point.
For example, if a data structure `foo` contains two fields `baz` and `bar` which must be updated in a consistent manner then the following code would not required a mutex:

    foo.baz = 5;
    foo.bar = 10;

In this case there is no chance that another task that may also be updating `foo` would execute between the two lines of code.
By contrast consider the following code:

    foo.baz = 5;
    yield();
    foo.bar = 10;

In this example, the code performs a yield between the two lines of code, which means that another task could access the `foo` data structure and see an inconsistent view.
A mutex can be used to protect the `foo` data structure (note that this applies not only to a direct call to [<span class="api">yield</span>], but any code that might indirectly result in context switch).

The system designer defines the signals available in the system at configuration time.
Each mutex has a name specified the system designer.
Internally each mutex is identified by an integer value with the type [<span class="api">MutexId</span>].
The configuration tool creates a symbolic name `MUTEX_ID_<name>` for each mutex in the system.
The underlying numeric value is assigned by the configuration tool and should be considered opaque to the application programmer.

To acquire a mutex, a task calls the [<span class="api">mutex_lock</span>] API.
On return from this API, the calling task is the holder of the mutex.
A task should not attempt to acquire a mutex that it already holds;
doing so creates a deadlock.
When using the [<span class="api">mutex_lock</span>] API, the task is effectively blocked until the mutex is acquired.

In some cases, blocking until the mutex is acquired is not appropriate.
The RTOS provides an alternative [<span class="api">mutex_try_lock</span>] API.
This API attempts to acquire the mutex, but returns immediately with either success or failure.

In some cases, there may be external timing constraints on a task that make it appropriate to block while acquiring the mutex, but only for a limited amount of time.
The RTOS provides a [<span class="api">mutex_lock_timeout</span>] function for cases where this is appropriate.

A task releases a mutex by calling the [<span class="api">mutex_unlock</span>] API.
A task should only release a mutex that it has previously acquired with one of the lock functions.
When a mutex is released, one of the tasks that is attempting to acquire that mutex succeeds and becomes the holder of the mutex.

Assuming a mutex called called `foo` was available to protect the `foo` data-structure, the previous example could be written as:

    mutex_lock(MUTEX_ID_FOO);
    foo.baz = 5;
    yield();
    foo.bar = 10;
    mutex_unlock(MUTEX_ID_FOO);

This would ensure that the `foo` data structure is updated in a protected manner.
Any other task accessing the `foo` data structure must first take the same `foo` mutex for the mutex to be effective.

There is a potential hazard that a incorrectly programmed task may not unlock the mutex in a timely manner.
To address this hazard the RTOS provides a mutex watchdog mechanism.
The system design may specify a maximum holding time for each mutex in the system.
If a task does not release the mutex before the watchdog timer expires then a system error occurs.

As any other objects that are not interrupt events, mutexes and their related APIs can not be used by interrupt service routines.

/*| doc_api |*/
## Mutexes

### <span class="api">MutexId</span>

A [<span class="api">MutexId</span>] refers to a specific mutex.
The underlying type is an unsigned integer of a size large enough to represent all mutexes[^MutexId_width].
The [<span class="api">MutexId</span>] should be treated an an opaque value.
For all mutexes in the system the configuration tool creates a constant with the name `MUTEX_ID_<name>` that should be used in preference to raw values.

[^MutexId_width]: This is normally a `uint8_t`.

### `MUTEX_ID_<name>`

`MUTEX_ID_<name>` has the type [<span class="api">MutexId</span>].
A constant is created for each mutex in the system.
Note that *name* is the upper-case conversion of the mutex's name.

### <span class="api">mutex_lock</span>

<div class="codebox">void mutex_lock(MutexId mutex);</div>

The [<span class="api">mutex_lock</span>] API attempts to acquire to specified mutex.
If the mutex is available, the calling task acquires the mutex and [<span class="api">mutex_lock</span>] returns to the caller immediately.
If the mutex is not immediately available, the calling task blocks until the mutex is acquired.

### <span class="api">mutex_try_lock</span>

<div class="codebox">bool mutex_try_lock(MutexId mutex);</div>

The [<span class="api">mutex_try_lock</span>] API attempts to acquire the specified mutex.
If the mutex is not immediately available, the function returns false.
If the mutex is available, the calling task acquires it and the function returns true.

### <span class="api">mutex_lock_timeout</span>

<div class="codebox">bool mutex_lock_timeout(MutexId mutex, TicksRelative timeout);</div>

The [<span class="api">mutex_lock_timeout</span>] API attempts to acquire the mutex.
If the mutex is not immediately available, the function blocks until either the mutex is acquired or the specified `timeout` elapses.
The function returns false if a time out occurs and true if the mutex is successfully acquired.

### <span class="api">mutex_unlock</span>

<div class="codebox">void mutex_unlock(MutexId mutex);</div>

The [<span class="api">mutex_unlock</span>] API releases the specified mutex.
It is incorrect for a task to use this API with a mutex it has not acquired.

/*| doc_configuration |*/
## Mutex Configuration

### `mutexes`

The `mutexes` configuration is a list of mutex configuration objects.
See the [Mutex Configuration] section for details on configuring each task.

### `name`

This configuration item specifies the mutex's name.
Each mutex must have a unique name.
The name must be of an identifier type.
This is a mandatory configuration item with no default.

### `maximum_hold_time`

This configuration item specifies the maximum length of time that a mutex may be held for.
The hold time is specified in ticks, and the value must fit in a [<span class="api">TicksRelative</span>] type.
If the hold time is specified as zero, the holding time of the mutex is not monitored.
This configuration item is optional and defaults to zero.

/*| doc_footer |*/
