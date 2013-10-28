/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/
#include <stdint.h>
#include <stddef.h>

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/
{{#tasks}}
static uint32_t stack_{{idx}}[{{stack_size}}] __attribute__((aligned(8)));
{{/tasks}}

/*| function_like_macros |*/

/*| functions |*/

/*| public_functions |*/
