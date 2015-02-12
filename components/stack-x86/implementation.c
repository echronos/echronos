/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/
{{#stack_static}}
{{#tasks}}
static uint8_t stack_{{idx}}[{{stack_size}}];
{{/tasks}}
{{/stack_static}}

/*| function_like_macros |*/
{{#stack_static}}
#define stack_init()
{{/stack_static}}
{{^stack_static}}
/* Host operating systems typically enable memory protection mechanisms that prevent the stack pointer from referring
 * to static or dynamic memory.
 * This precludes in particular the default RTOS approach of using statically allocated memory for task stacks.
 * Therefore, all task stacks must reside in the normal stack area of the user-space process in which the RTOS
 * system runs.
 * This is achieved by allocating the task stacks on the stack of the main/start function before switching to the
 * first task. */
#define stack_init()\
{{#tasks}}
    uint8_t stack_{{idx}}[{{stack_size}}];\
{{/tasks}}
{{/stack_static}}

/*| functions |*/

/*| public_functions |*/
