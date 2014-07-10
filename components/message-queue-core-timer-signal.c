/*| schema |*/

/*| public_headers |*/

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/

/*| headers |*/

/*| object_like_macros |*/

/*| type_definitions |*/

/*| structure_definitions |*/

/*| extern_definitions |*/

/*| function_definitions |*/

/*| state |*/

/*| function_like_macros |*/
#define message_queue_core_block() {{prefix_func}}signal_wait({{prefix_const}}SIGNAL_ID__TASK_TIMER)
#define message_queue_core_block_timeout(timeout) {{prefix_func}}sleep((timeout))
#define message_queue_core_unblock(task) {{prefix_func}}signal_send((task), {{prefix_const}}SIGNAL_ID__TASK_TIMER)

/*| functions |*/

/*| public_functions |*/
