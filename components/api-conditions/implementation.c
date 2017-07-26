/*| headers |*/

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/
{{#mpu_enabled}}
extern void rtos_internal_elevate_privileges(void);
extern void rtos_internal_drop_privileges(void);
extern uint32_t rtos_internal_in_usermode(void);
{{/mpu_enabled}}

/*| function_declarations |*/

/*| state |*/
{{#mpu_enabled}}
uint32_t rtos_internal_api_depth[{{tasks.length}}] = {0};
{{/mpu_enabled}}

/*| function_like_macros |*/
{{^mpu_enabled}}
#define rtos_internal_api_begin()
#define rtos_internal_api_end()
{{/mpu_enabled}}

{{#mpu_enabled}}
#define rtos_internal_api_begin() \
    if(rtos_internal_in_usermode()) { \
        rtos_internal_elevate_privileges(); \
    } \
    ++rtos_internal_api_depth[rtos_internal_current_task];

#define rtos_internal_api_end() \
    if(--rtos_internal_api_depth[rtos_internal_current_task] == 0) { \
        rtos_internal_drop_privileges(); \
    }
{{/mpu_enabled}}
/*| functions |*/

/*| public_functions |*/

/*| public_privileged_functions |*/
