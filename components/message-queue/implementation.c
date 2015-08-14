/*| headers |*/

/*| object_like_macros |*/
#define MESSAGE_QUEUE_ID_NONE ((MessageQueueIdOption)UINT8_C(255))

/*| types |*/
typedef uint8_t MessageQueueIdOption;

/*| structures |*/
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

/*| extern_declarations |*/

/*| function_declarations |*/

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
{{^message_queues.length}}
#define message_queue_init() do {} while(0)
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

    /* The timer of the current task is expected to be disabled.
     * It is expected to be only enabled while the current task is blocked in message_queue_wait_timeout().
     * Unfortunately, we cannot make any assumptions about the relationship between the states of message queues and
     * other timers.
     * The timers of tasks depends not only on the message queue implementation but also on how other components use
     * those task timers. */
    internal_assert(!timers[task_timers[get_current_task()]].enabled,\
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TIMER_IS_ENABLED);
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
/* called memcopy instead of memcpy to not conflict with gcc's built-in memcpy declaration on unit test targets */
static void
memcopy(uint8_t *dst, const uint8_t *src, const uint8_t length)
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
            memcopy(&mq->messages[buffer_offset], message, mq->message_size);
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
            memcopy((uint8_t*)message, &mq->messages[buffer_offset], mq->message_size);
            mq->head = (mq->head + 1) % mq->queue_length;
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
