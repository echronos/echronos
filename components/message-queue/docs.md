/*| doc_header |*/

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

/*| doc_footer |*/
