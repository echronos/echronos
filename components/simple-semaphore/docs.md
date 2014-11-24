/*| doc_header |*/

/*| doc_concepts |*/
## Semaphores

Semaphores provide an alternative signalling mechanism.
Conceptually a semaphore is an integral value with two operations: post and wait.
Post is sometimes called V, signal, or up.
Wait is sometimes called P or down.
The post operation increments the underlying value, whereas the wait operation decrements the underlying value, if (and only if) the current value of the semaphore is greater than zero.
The wait operation will block until the semaphore value becomes greater than zero.

Unlike a mutex, a semaphore has no concept of a holder.
A consequence of this is that when a task waits on a semaphore, no task will inherit the priority.

The post operation is made available through the [<span class="api">sem_post</span>] API.
The wait operation is made available through the [<span class="api">sem_wait</span>] API.
Additionally, a [<span class="api">sem_try_wait</span>] API is made available.
This allows a task to attempt to decrement a semaphore without blocking.

/*| doc_api |*/
## Semaphores

### <span class="api">SemId</span>

A [<span class="api">SemId</span>] refers to a specific semaphore.
The underlying type is an unsigned integer of a size large enough to represent all semaphores[^SemId_width].
The [<span class="api">SemId</span>] should be treated an an opaque value.
For all semaphores in the system the configuration tool creates a constant with the name `SEM_ID_<name>` that should be used in preference to raw values.

[^SemId_width]: This is normally a `uint8_t`.

### <span class="api">SemValue</span>

[<span class="api">SemValue</span>] is the type that is used to represent a semaphore's value.
The [<span class="api">SemValue</span>] type is also used to represent a semaphore's maximum value, if the RTOS is configured to allow maxima to be configured via the [<span class="api">sem_max_init</span>] API.
The width of [<span class="api">SemValue</span>] is 8, 16, or 32 (default 8) depending on RTOS configuration.
The underlying type is an unsigned integer of the appropriate size.

### <span class="api">sem_max_init</span>

<div class="codebox">void sem_max_init(SemId sem, SemValue max);</div>

The [<span class="api">sem_max_init</span>] API initializes the specified semaphore with the given maximum value, which must be non-zero.
The user must only call this API once per semaphore, and they must do this prior to the first time the [<span class="api">sem_post</span>] API is called for that semaphore.

### <span class="api">sem_post</span>

<div class="codebox">void sem_post(SemId sem);</div>

The [<span class="api">sem_post</span>] API increments the value of the specified semaphore.
If tasks are blocked on the semaphore then this implies a yield operation.

If the RTOS is configured to allow maxima to be configured via the [<span class="api">sem_max_init</span>] API, the [<span class="api">sem_post</span>] API must not be called for a semaphore prior to its maximum being initialized.
Subsequently, if a [<span class="api">sem_post</span>] causes the semaphore's value to reach its maximum, the RTOS will raise a fatal error.

### <span class="api">sem_wait</span>

<div class="codebox">void sem_wait(SemId sem);</div>

The [<span class="api">sem_wait</span>] API attempts to decrement the value of the specified semaphore.
If the semaphore value is positive then the value is decremented and [<span class="api">sem_wait</span>] returns to the caller immediately.
Otherwise the calling task will block until the semaphore value can be successfully decremented.
If the task blocks, then this function implies a yield operation, otherwise it does not.

### <span class="api">sem_try_wait</span>

<div class="codebox">bool sem_try_wait(SemId sem);</div>

The [<span class="api">sem_try_wait</span>] API attempts to decrement the value of the specified semaphore.
If the semaphore value is positive then the value is decremented and [<span class="api">sem_try_wait</span>] returns true.
Otherwise the function returns false and the semaphore value is not modified.
This function does not imply a yield operation.

/*| doc_configuration |*/
## Semaphore Configuration

### `semaphore_value_size`

This specifies the width of the semaphore value type.
It should be 8, 16 or 32.
This is an optional configuration item that defaults to 8.

### `semaphore_enable_max`

This boolean value enables mandatory runtime initialization of a maximum value for each semaphore via the [<span class="api">sem_max_init</span>] API.
If enabled, each semaphore must be initialized once with a non-zero maximum value using the [<span class="api">sem_max_init</span>] API, prior to the first time it is posted to via the [<span class="api">sem_post</span>] API.
Subsequently, if a [<span class="api">sem_post</span>] causes the semaphore's value to reach its maximum, the RTOS will raise a fatal error.
This is an optional configuration item that defaults to false.

### `semaphores`

The `semaphores` configuration is a list of semaphore configuration objects.
See the [Semaphore Configuration] section for details on configuring each semaphore.

### `name`

This configuration item specifies the semaphore's name.
Each semaphore must have a unique name.
The name must be of an identifier type.
This is a mandatory configuration item with no default.

/*| doc_footer |*/
