/*| headers |*/

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/
{{#memory_protection}}
extern void rtos_internal_elevate_privileges(void);
extern void rtos_internal_drop_privileges(void);
extern uint32_t rtos_internal_in_usermode(void);
{{/memory_protection}}

/*| function_declarations |*/

/*| state |*/
{{#memory_protection}}
uint32_t rtos_internal_api_depth[{{tasks.length}}] = {0};
{{/memory_protection}}

/*| function_like_macros |*/
{{^memory_protection}}
#define rtos_internal_api_begin()
#define rtos_internal_api_end()
{{/memory_protection}}

{{#memory_protection}}
#define rtos_internal_api_begin() \
    if(rtos_internal_in_usermode()) { \
        rtos_internal_elevate_privileges(); \
    } \
    ++rtos_internal_api_depth[rtos_internal_current_task];

#define rtos_internal_api_end() \
    if(--rtos_internal_api_depth[rtos_internal_current_task] == 0) { \
        rtos_internal_drop_privileges(); \
    }
{{/memory_protection}}
/*| functions |*/

/*| public_functions |*/

/*| public_privileged_functions |*/
