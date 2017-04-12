/* Applications do not necessarily access all RTOS APIs.
 * Therefore, they are marked as potentially unused for static analysis. */
/*| public_headers |*/
#include <stdbool.h>
#include <stdint.h>

/*| public_types |*/
typedef uint8_t {{prefix_type}}MessageQueueId;

/*| public_structures |*/

/*| public_object_like_macros |*/
{{#message_queues}}
#define {{prefix_const}}MESSAGE_QUEUE_ID_{{name|u}} (({{prefix_type}}MessageQueueId) UINT8_C({{idx}}))
{{/message_queues}}

/*| public_function_like_macros |*/

/*| public_state |*/

/*| public_function_declarations |*/
{{#message_queues.length}}
/*@unused@*/
void {{prefix_func}}message_queue_put({{prefix_type}}MessageQueueId message_queue, const void *message)
        {{prefix_const}}REENTRANT;
/*@unused@*/
bool {{prefix_func}}message_queue_try_put({{prefix_type}}MessageQueueId message_queue, const void *message);
/*@unused@*/
bool {{prefix_func}}message_queue_put_timeout({{prefix_type}}MessageQueueId message_queue, const void *message,
                                              {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT;
/*@unused@*/
void {{prefix_func}}message_queue_get({{prefix_type}}MessageQueueId message_queue, void *message)
        {{prefix_const}}REENTRANT;
/*@unused@*/
bool {{prefix_func}}message_queue_try_get({{prefix_type}}MessageQueueId message_queue, void *message);
/*@unused@*/
bool {{prefix_func}}message_queue_get_timeout({{prefix_type}}MessageQueueId message_queue, void *message,
                                              {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT;

{{/message_queues.length}}


/*| public_privileged_function_declarations |*/
