/*<module>
  <code_gen>template</code_gen>
  <headers>
    <header path="rtos-simple-mutex-test.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="num_mutexes" type="int"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
  </schema>
</module>*/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "rtos-simple-mutex-test.h"

[[mutex.headers]]

[[mutex.object_like_macros]]

[[mutex.type_definitions]]

[[mutex.structure_definitions]]

[[mutex.state]]

[[mutex.function_like_macros]]

[[mutex.functions]]

[[mutex.public_functions]]

void (*yield_ptr)(void);
struct mutex * pub_mutexes = mutexes;

void pub_set_yield_ptr(void (*y)(void))
{
    yield_ptr = y;
}

void {{prefix}}yield(void)
{
    if (yield_ptr != NULL)
    {
        yield_ptr();
    }
}
