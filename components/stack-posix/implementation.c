/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/

/*| types |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/
{{#tasks}}
static uint8_t stack_{{idx}}[{{stack_size}}];
{{/tasks}}

/*| function_like_macros |*/

/*| functions |*/

/*| public_functions |*/
