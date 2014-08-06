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

/*| functions |*/
{{#message_queues.length}}
static void
message_queue_waiters_wakeup(const {{prefix_type}}MessageQueueId message_queue)
{
    {{prefix_type}}TaskId task;

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
    message_queue_waiters[get_current_task()] = message_queue;
    message_queue_core_block();
}

static void
message_queue_wait_timeout(const {{prefix_type}}MessageQueueId message_queue,
                           const {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT
{
    message_queue_waiters[get_current_task()] = message_queue;
    message_queue_core_block_timeout(timeout);
    message_queue_waiters[get_current_task()] = MESSAGE_QUEUE_ID_NONE;
}

/* assumptions: max length 255, no overlap of dst & src */
static void
memcpy(uint8_t *dst, const uint8_t *src, const uint8_t length)
{
    uint8_t *const dst_end = dst + length;

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
    while (!{{prefix_func}}message_queue_try_put(message_queue, message))
    {
        message_queue_wait(message_queue);
    }
}

bool
{{prefix_func}}message_queue_try_put(const {{prefix_type}}MessageQueueId message_queue, const void *message)
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

bool
{{prefix_func}}message_queue_put_timeout(const {{prefix_type}}MessageQueueId message_queue, const void *const message,
                                         const {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TicksAbsolute absolute_timeout = {{prefix_func}}timer_current_ticks + timeout;

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
    while (!{{prefix_func}}message_queue_try_get(message_queue, message))
    {
        message_queue_wait(message_queue);
    }
}

bool
{{prefix_func}}message_queue_try_get(const {{prefix_type}}MessageQueueId message_queue, void *message)
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

bool
{{prefix_func}}message_queue_get_timeout(const {{prefix_type}}MessageQueueId message_queue, void *const message,
                                         const {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TicksAbsolute absolute_timeout = {{prefix_func}}timer_current_ticks + timeout;

    while ((message_queues[message_queue].available == 0) &&
            (absolute_timeout > {{prefix_func}}timer_current_ticks))
    {
        message_queue_wait_timeout(message_queue, absolute_timeout - {{prefix_func}}timer_current_ticks);
    }

    return {{prefix_func}}message_queue_try_get(message_queue, message);
}

{{/message_queues.length}}
