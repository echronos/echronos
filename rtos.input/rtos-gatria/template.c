/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-gatria.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="num_tasks" type="int"/>
   <entry name="num_mutexes" type="int"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="tasks" type="list">
     <entry name="task" type="dict">
      <entry name="idx" type="int" />
      <entry name="entry" type="c_ident" />
      <entry name="name" type="c_ident" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
   <entry name="mutexes" type="list">
     <entry name="mutex" type="dict">
      <entry name="idx" type="int" />
      <entry name="name" type="c_ident" />
     </entry>
   </entry>
  </schema>
</module>*/
/* Headers */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "rtos-gatria.h"
[[ALL.headers]]

/* Object-like macros */
[[ALL.object_like_macros]]

/* Type definitions */
[[ALL.type_definitions]]

/* Structure definitions */
[[ALL.structure_definitions]]

/* External definitions */
[[ALL.extern_definitions]]

/* Internal interface definitions */
[[ALL.function_definitions]]

/* State */
[[ALL.state]]

/* Function-like macros */
[[ALL.function_like_macros]]

/* Private functions */
[[ALL.functions]]

/* Public functions */
[[ALL.public_functions]]
