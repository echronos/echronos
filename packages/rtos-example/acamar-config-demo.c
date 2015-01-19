#include <stdint.h>
#include <debug.h>

#include "rtos-acamar.h"

{{#rtos.tasks}}
void
{{function}}(void)
{
    for (;;)
    {
        rtos_yield_to(({{rtos.prefix_const}}TASK_ID_{{name|u}} + 1) % {{rtos.tasks.length}});
        debug_println("task {{name}}");
    }
}
{{/rtos.tasks}}

int
main(void)
{
    rtos_start();
    for (;;) ;
}
