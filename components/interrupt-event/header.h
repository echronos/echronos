/*| public_headers |*/
{{#interrupt_events.length}}
#include <stdint.h>
{{/interrupt_events.length}}

/*| public_type_definitions |*/
{{#interrupt_events.length}}
typedef uint{{interrupteventid_size}}_t {{prefix_type}}InterruptEventId;
{{/interrupt_events.length}}

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#interrupt_events}}
#define {{prefix_const}}INTERRUPT_EVENT_ID_{{name|u}} (({{prefix_type}}InterruptEventId) UINT{{interrupteventid_size}}_C({{idx}}))
{{/interrupt_events}}

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
