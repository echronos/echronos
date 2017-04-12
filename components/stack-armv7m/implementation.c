/*| headers |*/
#include <stdint.h>
#include <stddef.h>

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/
{{#memory_protection}}
/* We need to align stack addresses on their size to create a valid MPU region.
 * Additionally, we place the stack in a specially named section so that
 * we are able to see where the linker places it and configure the MPU accordingly. */
{{/memory_protection}}
{{#tasks}}
{{#memory_protection}}
static uint32_t stack_{{idx}}[{{stack_size}}]
    __attribute__(( aligned({{stack_size}}*sizeof(uint32_t)) ))
    __attribute__(( section (".bss.stack.task_{{idx}}") ));
{{/memory_protection}}
{{^memory_protection}}
static uint32_t stack_{{idx}}[{{stack_size}}] __attribute__((aligned(8)));
{{/memory_protection}}
{{/tasks}}

/*| function_like_macros |*/

/*| functions |*/

/*| public_functions |*/

/*| public_privileged_functions |*/
