/* Applications do not necessarily access all RTOS APIs.
 * Therefore, they are marked as potentially unused for static analysis. */
/*| public_headers |*/

/*| public_types |*/

/*| public_structures |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_state |*/

/*| public_function_declarations |*/
/*@unused@*/
void {{prefix_func}}yield(void) {{prefix_const}}REENTRANT;
void {{prefix_func}}start(void);

/*| public_privileged_function_declarations |*/
