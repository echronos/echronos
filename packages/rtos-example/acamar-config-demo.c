#include <stdint.h>
#include <debug.h>

#include "rtos-acamar.h"

{{#tasks}}
void
{{function}}(void)
{
    for (;;)
    {
        rtos_yield_to(({{prefix_const}}TASK_ID_{{name|u}} + 1) % {{tasks.length}});
        debug_println("task {{name}}");
    }
}
{{/tasks}}

int
main(void)
{
    rtos_start();
    for (;;) ;
}
