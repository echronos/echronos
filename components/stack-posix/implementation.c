/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/
{{#tasks}}
static uint8_t stack_{{idx}}[{{stack_size}}];
{{/tasks}}

/*| function_like_macros |*/

/*| functions |*/

/*| public_functions |*/
