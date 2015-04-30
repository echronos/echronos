/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/
/*
 * PowerPC EABI stack is 8-byte aligned.
 * See ppce500-context-switch.c
 */
{{#tasks}}
static uint32_t stack_{{idx}}[{{stack_size}}] __attribute__((aligned(8)));
{{/tasks}}

/*| function_like_macros |*/

/*| functions |*/

/*| public_functions |*/
