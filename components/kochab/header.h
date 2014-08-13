/*| schema |*/
<entry name="prefix" type="ident" optional="true" />
<entry name="tasks" type="list" auto_index_field="idx">
    <entry name="task" type="dict">
        <entry name="priority" type="int" />
    </entry>
</entry>

/*| public_headers |*/
#include <stdint.h>

/*| public_type_definitions |*/

/*| public_structure_definitions |*/

/*| public_object_like_macros |*/

/*| public_function_like_macros |*/

/*| public_extern_definitions |*/

/*| public_function_definitions |*/
void {{prefix_func}}start(void);
