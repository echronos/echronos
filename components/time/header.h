/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/
typedef uint8_t {{prefix_type}}TimerId;
typedef uint32_t {{prefix_type}}TicksAbsolute;
typedef uint16_t {{prefix_type}}TicksRelative;

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/
{{#timers}}
#define {{prefix_const}}TIMER_ID_{{name|u}} (({{prefix_type}}TimerId) UINT8_C({{idx}}))
{{/timers}}
/* This old naming is deprecated */
#define {{prefix_func}}timer_current_ticks {{prefix_func}}time_current_ticks

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/
extern {{prefix_type}}TicksAbsolute {{prefix_func}}time_current_ticks;

/*| public_function_definitions |*/
