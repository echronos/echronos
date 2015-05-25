/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/
#define ERROR_ID_NONE (({{prefix_type}}ErrorId) UINT8_C(0))
#define ERROR_ID_TICK_OVERFLOW (({{prefix_type}}ErrorId) UINT8_C(1))
#define ERROR_ID_INVALID_ID (({{prefix_type}}ErrorId) UINT8_C(2))
#define ERROR_ID_NOT_HOLDING_MUTEX (({{prefix_type}}ErrorId) UINT8_C(3))
#define ERROR_ID_DEADLOCK (({{prefix_type}}ErrorId) UINT8_C(4))
#define ERROR_ID_TASK_FUNCTION_RETURNS (({{prefix_type}}ErrorId) UINT8_C(5))
#define ERROR_ID_INTERNAL_CURRENT_TASK_INVALID (({{prefix_type}}ErrorId) UINT8_C(6))
#define ERROR_ID_INTERNAL_INVALID_ID (({{prefix_type}}ErrorId) UINT8_C(7))
#define ERROR_ID_MESSAGE_QUEUE_BUFFER_OVERLAP (({{prefix_type}}ErrorId) UINT8_C(8))
#define ERROR_ID_MESSAGE_QUEUE_ZERO_TIMEOUT (({{prefix_type}}ErrorId) UINT8_C(9))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_ZERO_TIMEOUT (({{prefix_type}}ErrorId) UINT8_C(10))
#define ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER (({{prefix_type}}ErrorId) UINT8_C(11))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW (({{prefix_type}}ErrorId) UINT8_C(12))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION (({{prefix_type}}ErrorId) UINT8_C(13))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_CONFIGURATION (({{prefix_type}}ErrorId) UINT8_C(14))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_HEAD (({{prefix_type}}ErrorId) UINT8_C(15))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_AVAILABLE (({{prefix_type}}ErrorId) UINT8_C(16))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_ID_IN_WAITERS (({{prefix_type}}ErrorId) UINT8_C(17))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TASKS_BLOCKED_DESPITE_AVAILABLE_MESSAGES (({{prefix_type}}ErrorId) UINT8_C(18))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_WAITING_TASK_IS_NOT_BLOCKED (({{prefix_type}}ErrorId) UINT8_C(19))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGES_POINTER (({{prefix_type}}ErrorId) UINT8_C(20))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGE_SIZE (({{prefix_type}}ErrorId) UINT8_C(21))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_QUEUE_LENGTH (({{prefix_type}}ErrorId) UINT8_C(22))
#define ERROR_ID_INTERNAL_PRECONDITION_VIOLATED (({{prefix_type}}ErrorId) UINT8_C(23))
#define ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED (({{prefix_type}}ErrorId) UINT8_C(24))
#define ERROR_ID_SEMAPHORE_MAX_INVALID (({{prefix_type}}ErrorId) UINT8_C(25))
#define ERROR_ID_SEMAPHORE_MAX_USE_BEFORE_INIT (({{prefix_type}}ErrorId) UINT8_C(26))
#define ERROR_ID_SEMAPHORE_MAX_ALREADY_INIT (({{prefix_type}}ErrorId) UINT8_C(27))
#define ERROR_ID_SEMAPHORE_MAX_EXCEEDED (({{prefix_type}}ErrorId) UINT8_C(28))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TIMER_IS_ENABLED (({{prefix_type}}ErrorId) UINT8_C(29))
#define ERROR_ID_SCHED_PRIO_CEILING_TASK_LOCKING_LOWER_PRIORITY_MUTEX (({{prefix_type}}ErrorId) UINT8_C(30))
#define ERROR_ID_SCHED_PRIO_CEILING_MUTEX_ALREADY_LOCKED (({{prefix_type}}ErrorId) UINT8_C(31))

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/
extern void {{fatal_error}}({{prefix_type}}ErrorId error_id);

/*| function_declarations |*/

/*| state |*/

/*| function_like_macros |*/
{{#api_asserts}}
#define api_error(error_id) {{fatal_error}}(error_id)
{{/api_asserts}}
{{^api_asserts}}
#define api_error(error_id) do { } while(0)
{{/api_asserts}}
{{#api_asserts}}
#define api_assert(expression, error_id) do { if (!(expression)) { api_error(error_id); } } while(0)
{{/api_asserts}}
{{^api_asserts}}
#define api_assert(expression, error_id) do { } while(0)
{{/api_asserts}}

{{#internal_asserts}}
#define internal_error(error_id) {{fatal_error}}(error_id)
{{/internal_asserts}}
{{^internal_asserts}}
#define internal_error(error_id) do { } while(0)
{{/internal_asserts}}
{{#internal_asserts}}
#define internal_assert(expression, error_id) do { if (!(expression)) { internal_error(error_id); } } while(0)
{{/internal_asserts}}
{{^internal_asserts}}
#define internal_assert(expression, error_id) do { } while(0)
{{/internal_asserts}}

/*| functions |*/

/*| public_functions |*/
