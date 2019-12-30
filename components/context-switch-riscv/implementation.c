/*| headers |*/
#include <stdint.h>
#include <stddef.h>

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/
{{#mpu_enabled}}
/* We need to align stack addresses by stack size to create a valid MPU region.
 * Additionally, we place the stack in a specially named section so that
 * we are able to see where the linker places it and configure the MPU accordingly. */
{{/mpu_enabled}}
{{#tasks}}
{{#mpu_enabled}}
static uint32_t stack_{{idx}}[{{stack_size}}]
    __attribute__(( aligned({{stack_size}}*sizeof(uint32_t)) ))
    __attribute__(( section (".bss.stack.task_{{idx}}") ));
{{/mpu_enabled}}
{{^mpu_enabled}}
static uint32_t stack_{{idx}}[{{stack_size}}] __attribute__((aligned(8)));
{{/mpu_enabled}}
{{/tasks}}

/*| function_like_macros |*/

/*| functions |*/

/*| public_functions |*/

/*| public_privileged_functions |*/
