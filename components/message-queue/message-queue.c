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
void {{prefix_func}}message_queue_put({{prefix_type}}MessageQueueId message_queue, const void *message);
bool {{prefix_func}}message_queue_try_put({{prefix_type}}MessageQueueId message_queue, const void *message);
bool {{prefix_func}}message_queue_put_timeout({{prefix_type}}MessageQueueId message_queue, const void *message, {{prefix_type}}TicksRelative timeout);
void {{prefix_func}}message_queue_get({{prefix_type}}MessageQueueId message_queue, void *message);
bool {{prefix_func}}message_queue_try_get({{prefix_type}}MessageQueueId message_queue, void *message);
bool {{prefix_func}}message_queue_get_timeout({{prefix_type}}MessageQueueId message_queue, void *message, {{prefix_type}}TicksRelative timeout);

{{/message_queues.length}}

/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

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

{{/message_queues.length}}

/*| function_like_macros |*/

/*| functions |*/

/*| public_functions |*/
{{#message_queues.length}}
void {{prefix_func}}message_queue_put(const {{prefix_type}}MessageQueueId message_queue, const void *const message)
{
    (void){{prefix_func}}message_queue_put_timeout(message_queue, message, 0);
}

bool {{prefix_func}}message_queue_try_put(const {{prefix_type}}MessageQueueId message_queue, const void *message)
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
        uint8_t *buffer;
        for (buffer = &mq->messages[buffer_offset]; buffer < &mq->messages[buffer_offset + mq->message_size]; buffer += 1)
        {
            *buffer = *(uint8_t*)message++;
        }
        mq->available += 1;
        return true;
    }
}

bool {{prefix_func}}message_queue_put_timeout(const {{prefix_type}}MessageQueueId message_queue, const void *const message, const {{prefix_type}}TicksRelative timeout)
{
    const {{prefix_type}}TicksAbsolute absolute_timeout = {{prefix_func}}timer_current_ticks + timeout;

    while (!{{prefix_func}}message_queue_try_put(message_queue, message))
    {
        /* The !timeout part is not meant to be final; it just allows an implementation shortcut for this prototype */
        if (!timeout || {{prefix_func}}timer_current_ticks < absolute_timeout)
        {
            {{prefix_func}}yield();
        }
        else
        {
            return false;
        }
    }

    return true;
}

void {{prefix_func}}message_queue_get(const {{prefix_type}}MessageQueueId message_queue, void *const message)
{
    (void){{prefix_func}}message_queue_get_timeout(message_queue, message, 0);
}

bool {{prefix_func}}message_queue_try_get(const {{prefix_type}}MessageQueueId message_queue, void *message)
{
    struct message_queue *const mq = &message_queues[message_queue];

    if (mq->available == 0)
    {
        return false;
    }
    else
    {
        const uint16_t buffer_offset = mq->head * mq->message_size;
        uint8_t *buffer;
        for (buffer = &mq->messages[buffer_offset]; buffer < &mq->messages[buffer_offset + mq->message_size]; buffer += 1)
        {
            *(uint8_t*)message++ = *buffer;
        }
        mq->available -= 1;
        return true;
    }
}

bool {{prefix_func}}message_queue_get_timeout(const {{prefix_type}}MessageQueueId message_queue, void *const message, const {{prefix_type}}TicksRelative timeout)
{
    const {{prefix_type}}TicksAbsolute absolute_timeout = {{prefix_func}}timer_current_ticks + timeout;

    while (!{{prefix_func}}message_queue_try_get(message_queue, message))
    {
        /* The !timeout part is not meant to be final; it just allows an implementation shortcut for this prototype */
        if ((!timeout) || ({{prefix_func}}timer_current_ticks < absolute_timeout))
        {
            {{prefix_func}}yield();
        }
        else
        {
            return false;
        }
    }

    return true;
}

{{/message_queues.length}}
