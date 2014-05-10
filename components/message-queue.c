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
{{#mutexes.length}}
#define message_queue_api_assert_valid(message_queue) api_assert(message_queue < {{message_queues.length}}, ERROR_ID_INVALID_ID)
#define message_queue_internal_assert_valid(message_queue) api_assert(message_queue < {{message_queues.length}}, ERROR_ID_INVALID_ID)
{{^internal_asserts}}
#define message_queue_init() do {} while(0)
{{/internal_asserts}}


{{/mutexes.length}}

/*| functions |*/
{{#message_queues.length}}
{{#internal_asserts}}
static void message_queue_init(void)
{
    {{prefix_type}}MessageQueueId message_queue;
    {{prefix_type}}TaskId task;

    for (message_queue = 0; message_queue < {{message_queues.length}}; message_queue += 1)
    {
        const struct message_queue *const mq = &message_queues[message_queue];

        internal_assert(mq->messages &&
                        mq->message_size &&
                        mq->queue_length &&
                        !mq->head &&
                        !mq->available, ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION);
    }

    for (task = 0; task <= TASK_ID_MAX; task += 1)
    {
        internal_assert(message_queue_waiters[task] == MESSAGE_QUEUE_ID_NONE, ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION);
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

    message_queue_waiters[get_current_task()] = message_queue;
    message_queue_core_block();
}

static void
message_queue_wait_timeout(const {{prefix_type}}MessageQueueId message_queue,
                           const {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT
{
    message_queue_internal_assert_valid(message_queue);
    internal_assert(timeout, ERROR_ID_MESSAGE_QUEUE_INTERNAL_ZERO_TIMEOUT);

    message_queue_waiters[get_current_task()] = message_queue;
    message_queue_core_block_timeout(timeout);
    message_queue_waiters[get_current_task()] = MESSAGE_QUEUE_ID_NONE;
}

/* assumptions: max length 255, no overlap of dst & src */
static void
memcpy(uint8_t *dst, const uint8_t *src, const uint8_t length)
{
    uint8_t *const dst_end = dst + length;

    api_assert((dst < src) || (dst > (src + length)), ERROR_ID_MESSAGE_QUEUE_BUFFER_OVERLAP);

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
    internal_assert({{prefix_func}}timer_current_ticks < (UINT32_MAX - timeout), ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW);

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
    internal_assert({{prefix_func}}timer_current_ticks < (UINT32_MAX - timeout), ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW);

    while ((message_queues[message_queue].available == 0) &&
            (absolute_timeout > {{prefix_func}}timer_current_ticks))
    {
        message_queue_wait_timeout(message_queue, absolute_timeout - {{prefix_func}}timer_current_ticks);
    }

    return {{prefix_func}}message_queue_try_get(message_queue, message);
}

{{/message_queues.length}}
