/*| public_headers |*/

/*| public_types |*/

/*| public_structures |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_state |*/

/*| public_function_declarations |*/
void {{prefix_func}}yield_to({{prefix_type}}TaskId to) {{prefix_const}}REENTRANT;
void {{prefix_func}}yield(void) {{prefix_const}}REENTRANT;
void {{prefix_func}}block(void) {{prefix_const}}REENTRANT;
void {{prefix_func}}unblock({{prefix_type}}TaskId task);
void {{prefix_func}}start(void);
