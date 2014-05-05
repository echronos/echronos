/*| schema |*/
<entry name="api_asserts" type="bool" default="false" />
<entry name="internal_asserts" type="bool" default="false" />
<entry name="fatal_error" type="c_ident" optional="true" />

/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t {{prefix_type}}ErrorId;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

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

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/
{{#api_asserts}}
extern void {{fatal_error}}({{prefix_type}}ErrorId error_id);
{{/api_asserts}}

/*| function_definitions |*/

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
