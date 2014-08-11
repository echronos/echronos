/*| dependencies |*/
signal
task
timer

/*| doc_concepts |*/
## Message Queues

Message queues provide a mechanism for transferring data between different tasks.
Each message queue is defined with a fixed size message, and a maximum numbers of messages that the queue can hold.

The RTOS provides a set of APIs to put a message in to the message queue, and to get a message from a message queue.
The put and get operations copy the message to and from the message queue storage.

Message queues operate in a first-in first-out basis.

/*| doc_api |*/
## Message Queues

### <span class="api">MessageQueueId</span>

A [<span class="api">MessageQueueId</span>] refers to a specific message queue.
The underlying type is an unsigned integer of a size large enough to represent all message queues[^MessageQueueId_width].
The [<span class="api">MessageQueueId</span>] should be treated an an opaque value.
For all message queues in the system the configuration tool creates a constant with the name `MESSAGE_QUEUE_ID_<name>` that should be used in preference to raw values.

[^MessageQueueId_width]: This is normally a `uint8_t`.

### `MESSAGE_QUEUE_ID_<name>`

`MESSAGE_QUEUE_ID_<name>` has the type MessageQueue.
A constant is created for each message queue in the system.
Note that *name* is the upper-case conversion of the message queue's name.

### <span class="api">message_queue_put</span>

<div class="codebox">void message_queue_put(MessageQueueId message_queue, void *message);</div>

The [<span class="api">message_queue_put</span>] API places a new message on the end of a message queue.
The `message` pointer must point to a buffer of the correct size.
The correct message size is based on the configured item size for the message queue.
If the message queue is full, the calling task blocks until there is space in the message queue.

### <span class="api">message_queue_put_try</span>

<div class="codebox">bool message_queue_put_try(MessageQueueId message_queue, void *message);</div>

The [<span class="api">message_queue_put_try</span>] API attempts to put a new message on the end of the specified message queue.
The message pointer must point to a buffer of the correct size.
The correct message size is based on the configured item size for the message queue.
If there is space on the message queue, the function returns true, otherwise the function returns false.

### <span class="api">message_queue_put_timeout</span>

<div class="codebox">bool message_queue_put_timeout(MessageQueueId message_queue, void *message, TicksRelative timeout);</div>

The [<span class="api">message_queue_put_timeout</span>] API attempts to put a new message on the end of the specified message queue.
The message pointer must point to a buffer of the correct size.
The correct message size is based on the configure item size for the message queue.
If the message queue is full, the calling task blocks until there is space in the message queue for a maximum of `timeout` ticks.
If the message is put onto the queue before `timeout` ticks elapse, the function returns true.
If the timeout expires, the function returns false.

### <span class="api">message_queue_get</span>

<div class="codebox">void message_queue_get(MessageQueueId message_queue, void *message);</div>

The [<span class="api">message_queue_get</span>] API checks whether a message is available in the message queue `message_queue`.
If no message is available, the API blocks until a message is available.
Once a message is available, the API copies the data of the oldest message in message_queue to the memory location `message`.
The API copies as many bytes of data as what is configured as message size for message_queue.
The API then removes the message from `message_queue`.

### <span class="api">message_queue_get_try</span>

<div class="codebox">bool message_queue_get_try(MessageQueueId message_queue, void *message);</div>

The [<span class="api">message_queue_get_try</span>] API checks whether a message is available in the message queue `message_queue`.
If no message is available, it returns false.
If a message is available, it behaves exactly as the [<span class="api">message_queue_get</span>] API would and returns true.

### <span class="api">message_queue_get_timeout</span>

<div class="codebox">bool message_queue_get_timeout(MessageQueueId message_queue, void *message, TicksRelative timeout);</div>

The [<span class="api">message_queue_get_timeout</span>] API checks whether a message is available in the message queue `message_queue`.
If a message is available, the API behaves exactly as the [<span class="api">message_queue_get_try</span>] API would.
If no message is available, the API blocks until either a message is available in `message_queue` or until timeout ticks pass.
Once either of the two cases occurs, the API behaves exactly as the [<span class="api">message_queue_get_try</span>] API would.

/*| doc_configuration |*/
## Message Queue Configuration

### `message_queues`

The `message_queues` configuration is a list of message queue configuration objects.

### `name`

This configuration item specifies the message queue's name.
Each message queue must have a unique name.
The name must be of an identifier type.
This is a mandatory configuration item with no default.

### `message_size`

This configuration item specifies the size of each message in the queue.
This is a mandatory configuration item with no default.

### `queue_length`

This configuration item specifies the length of the message queue.
This is a mandatory configuration item with no default.

/*| schema |*/
<entry name="message_queues" type="list" default="[]" auto_index_field="idx">
    <entry name="message_queue" type="dict">
        <entry name="name" type="ident" />
        <entry name="message_size" type="int" optional="true" />
        <entry name="message_type" type="string" optional="true" />
        <constraint name="constraint0" type="one_of">
            <entry name="message_size">message_size</entry>
            <entry name="message_type">message_type</entry>
        </constraint>
        <entry name="queue_length" type="int" />
    </entry>
</entry>

/*| public_headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t {{prefix_type}}MessageQueueId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#message_queues}}
#define {{prefix_const}}MESSAGE_QUEUE_ID_{{name|u}} (({{prefix_type}}MessageQueueId) UINT8_C({{idx}}))
{{/message_queues}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
{{#message_queues.length}}
void {{prefix_func}}message_queue_put({{prefix_type}}MessageQueueId message_queue, const void *message)
        {{prefix_const}}REENTRANT;
bool {{prefix_func}}message_queue_try_put({{prefix_type}}MessageQueueId message_queue, const void *message);
bool {{prefix_func}}message_queue_put_timeout({{prefix_type}}MessageQueueId message_queue, const void *message,
                                              {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT;
void {{prefix_func}}message_queue_get({{prefix_type}}MessageQueueId message_queue, void *message)
        {{prefix_const}}REENTRANT;
bool {{prefix_func}}message_queue_try_get({{prefix_type}}MessageQueueId message_queue, void *message);
bool {{prefix_func}}message_queue_get_timeout({{prefix_type}}MessageQueueId message_queue, void *message,
                                              {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT;

{{/message_queues.length}}

/*| headers |*/

/*| object_like_macros |*/
#define MESSAGE_QUEUE_ID_NONE ((MessageQueueIdOption)UINT8_C(255))

/*| type_definitions |*/
typedef uint8_t MessageQueueIdOption;

/*| structure_definitions |*/
/* representation of a message queue instance
 * sorted by size of fields */
struct message_queue
{
    /* pointer to the array holding the message data
     * the array contains message_size * queue_length bytes */
    uint8_t *messages;
    /* size of each message in bytes */
    const uint8_t message_size;
    /* maximum number of messages this queue can hold */
    const uint8_t queue_length;
    /* index of the oldest message that has been put into the queue but not yet been retrieved
     * 0 <= head < queue_length */
    uint8_t head;
    /* number of messages that have been put into the queue but not yet been retrieved
     * 0 <= available < queue_length */
    uint8_t available;
};

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/
{{#message_queues.length}}
{{#message_queues}}
static uint8_t message_queue_{{name}}_messages[{{queue_length}}][{{message_size}}];
{{/message_queues}}
static struct message_queue message_queues[] =
{
{{#message_queues}}
    {
        (uint8_t*)message_queue_{{name}}_messages,
{{#message_size}}
        {{message_size}},
{{/message_size}}
{{#message_type}}
        sizeof({{message_type}}),
{{/message_type}}
        {{queue_length}},
        0,
        0,
    },
{{/message_queues}}
};
static MessageQueueIdOption message_queue_waiters[] =
{
{{#tasks}}
    MESSAGE_QUEUE_ID_NONE,
{{/tasks}}
};

{{/message_queues.length}}

/*| function_like_macros |*/
{{#message_queues.length}}
#define message_queue_api_assert_valid(message_queue) api_assert(message_queue < {{message_queues.length}},\
                                                                 ERROR_ID_INVALID_ID)
#define message_queue_internal_assert_valid(message_queue) internal_assert(message_queue < {{message_queues.length}},\
                                                                           ERROR_ID_INVALID_ID)
{{^internal_asserts}}
#define message_queue_init() do {} while(0)
#define message_queue_invariants_check() do {} while(0)
{{/internal_asserts}}

{{/message_queues.length}}

/*| functions |*/
{{#message_queues.length}}
{{#internal_asserts}}
static void
message_queue_init(void)
{
    {{prefix_type}}MessageQueueId message_queue = {{message_queues.length}} - 1;
    {{prefix_type}}TaskId task;

    /* do not use for loop to work around buggy compiler optimization when there is only one message queue */
    do
    {
        struct message_queue *mq = &message_queues[message_queue];

        internal_assert(mq->messages &&
                        mq->message_size &&
                        mq->queue_length &&
                        !mq->head &&
                        !mq->available, ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION);
    } while (message_queue--);

    for (task = 0; task <= {{prefix_const}}TASK_ID_MAX; task += 1)
    {
        internal_assert(message_queue_waiters[task] == MESSAGE_QUEUE_ID_NONE,\
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION);
    }
}

static void
message_queue_invariants_check(void)
{
    {{prefix_type}}MessageQueueId message_queue;
    {{prefix_type}}TaskId task;

{{#message_queues}}
    internal_assert(message_queues[{{idx}}].messages == (uint8_t*)message_queue_{{name}}_messages,
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGES_POINTER);
{{#message_size}}
    internal_assert(message_queues[{{idx}}].message_size == {{message_size}},
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGE_SIZE);
{{/message_size}}
{{#message_type}}
    internal_assert(message_queues[{{idx}}].message_size == sizeof({{message_type}}),
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGE_SIZE);
{{/message_type}}
    internal_assert(message_queues[{{idx}}].queue_length == {{queue_length}},
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_QUEUE_LENGTH);
{{/message_queues}}

    for (message_queue = 0; message_queue < {{message_queues.length}}; message_queue += 1)
    {
        const struct message_queue *const mq = &message_queues[message_queue];

        internal_assert(mq->messages && mq->message_size && mq->queue_length,
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_CONFIGURATION);
        internal_assert(mq->head < mq->queue_length, ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_HEAD);
        internal_assert(mq->available <= mq->queue_length,
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_AVAILABLE);
    }

    for (task = 0; task <= {{prefix_const}}TASK_ID_MAX; task += 1)
    {
        message_queue = message_queue_waiters[task];

        internal_assert((message_queue < {{message_queues.length}}) || (message_queue == MESSAGE_QUEUE_ID_NONE),\
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_ID_IN_WAITERS);

        if (message_queue != MESSAGE_QUEUE_ID_NONE)
        {
            const struct message_queue *const mq = &message_queues[message_queue];

            internal_assert((mq->available == 0) || (mq->available == mq->queue_length),\
                ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TASKS_BLOCKED_DESPITE_AVAILABLE_MESSAGES);
            internal_assert(!message_queue_core_is_unblocked(task),\
                            ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_WAITING_TASK_IS_NOT_BLOCKED);
        }
    }
}

{{/internal_asserts}}
static void
message_queue_waiters_wakeup(const {{prefix_type}}MessageQueueId message_queue)
{
    {{prefix_type}}TaskId task;

    message_queue_internal_assert_valid(message_queue);

    for (task = {{prefix_const}}TASK_ID_ZERO; task <= {{prefix_const}}TASK_ID_MAX; task += 1)
    {
        if (message_queue_waiters[task] == message_queue)
        {
            message_queue_core_unblock(task);
            message_queue_waiters[task] = MESSAGE_QUEUE_ID_NONE;
        }
    }
}

static void
message_queue_wait(const {{prefix_type}}MessageQueueId message_queue) {{prefix_const}}REENTRANT
{
    message_queue_internal_assert_valid(message_queue);
    message_queue_invariants_check();

    message_queue_waiters[get_current_task()] = message_queue;
    message_queue_core_block();

    message_queue_invariants_check();
}

static void
message_queue_wait_timeout(const {{prefix_type}}MessageQueueId message_queue,
                           const {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT
{
    message_queue_internal_assert_valid(message_queue);
    internal_assert(timeout, ERROR_ID_MESSAGE_QUEUE_INTERNAL_ZERO_TIMEOUT);
    message_queue_invariants_check();

    message_queue_waiters[get_current_task()] = message_queue;
    message_queue_core_block_timeout(timeout);
    message_queue_waiters[get_current_task()] = MESSAGE_QUEUE_ID_NONE;

    message_queue_invariants_check();
}

/* assumptions: max length 255, no overlap of dst & src */
static void
memcpy(uint8_t *dst, const uint8_t *src, const uint8_t length)
{
    uint8_t *const dst_end = dst + length;

    api_assert((dst < src) || (dst >= (src + length)), ERROR_ID_MESSAGE_QUEUE_BUFFER_OVERLAP);

    while (dst < dst_end)
    {
        *dst++ = *src++;
    }
}

{{/message_queues.length}}

/*| public_functions |*/
{{#message_queues.length}}
void
{{prefix_func}}message_queue_put(const {{prefix_type}}MessageQueueId message_queue, const void *const message)
        {{prefix_const}}REENTRANT
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);

    while (!{{prefix_func}}message_queue_try_put(message_queue, message))
    {
        message_queue_wait(message_queue);
    }
}

bool
{{prefix_func}}message_queue_try_put(const {{prefix_type}}MessageQueueId message_queue, const void *message)
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    message_queue_invariants_check();

    {
        struct message_queue *const mq = &message_queues[message_queue];

        if (mq->available == mq->queue_length)
        {
            return false;
        }
        else
        {
            const uint8_t buffer_index = (mq->head + mq->available) % mq->queue_length;
            const uint16_t buffer_offset = buffer_index * mq->message_size;
            memcpy(&mq->messages[buffer_offset], message, mq->message_size);
            mq->available += 1;

            if (mq->available == 1)
            {
                message_queue_waiters_wakeup(message_queue);
            }

            message_queue_invariants_check();
            return true;
        }
    }
}

bool
{{prefix_func}}message_queue_put_timeout(const {{prefix_type}}MessageQueueId message_queue, const void *const message,
                                         const {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TicksAbsolute absolute_timeout = {{prefix_func}}timer_current_ticks + timeout;

    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    api_assert(timeout, ERROR_ID_MESSAGE_QUEUE_ZERO_TIMEOUT);
    internal_assert({{prefix_func}}timer_current_ticks < (UINT32_MAX - timeout),\
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW);
    message_queue_invariants_check();

    while ((message_queues[message_queue].available == message_queues[message_queue].queue_length) &&
            (absolute_timeout > {{prefix_func}}timer_current_ticks))
    {
        message_queue_wait_timeout(message_queue, absolute_timeout - {{prefix_func}}timer_current_ticks);
    }

    return {{prefix_func}}message_queue_try_put(message_queue, message);
}

void
{{prefix_func}}message_queue_get(const {{prefix_type}}MessageQueueId message_queue, void *const message)
        {{prefix_const}}REENTRANT
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);

    while (!{{prefix_func}}message_queue_try_get(message_queue, message))
    {
        message_queue_wait(message_queue);
    }
}

bool
{{prefix_func}}message_queue_try_get(const {{prefix_type}}MessageQueueId message_queue, void *message)
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    message_queue_invariants_check();

    {
        struct message_queue *const mq = &message_queues[message_queue];

        if (mq->available == 0)
        {
            return false;
        }
        else
        {
            const uint16_t buffer_offset = mq->head * mq->message_size;
            memcpy((uint8_t*)message, &mq->messages[buffer_offset], mq->message_size);
            mq->available -= 1;

            if (mq->available == ({{message_queues.length}} - 1))
            {
                message_queue_waiters_wakeup(message_queue);
            }

            message_queue_invariants_check();
            return true;
        }
    }
}

bool
{{prefix_func}}message_queue_get_timeout(const {{prefix_type}}MessageQueueId message_queue, void *const message,
                                         const {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TicksAbsolute absolute_timeout = {{prefix_func}}timer_current_ticks + timeout;

    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    api_assert(timeout, ERROR_ID_MESSAGE_QUEUE_ZERO_TIMEOUT);
    internal_assert({{prefix_func}}timer_current_ticks < (UINT32_MAX - timeout),\
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW);
    message_queue_invariants_check();

    while ((message_queues[message_queue].available == 0) &&
            (absolute_timeout > {{prefix_func}}timer_current_ticks))
    {
        message_queue_wait_timeout(message_queue, absolute_timeout - {{prefix_func}}timer_current_ticks);
    }

    return {{prefix_func}}message_queue_try_get(message_queue, message);
}

{{/message_queues.length}}
