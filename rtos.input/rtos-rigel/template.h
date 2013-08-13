#ifndef RTOS_RIGEL_H
#define RTOS_RIGEL_H

#include <stdint.h>
#include <stdbool.h>

typedef uint{{taskid_size}}_t TaskId;
[[irq_event.public_type_definitions]]
[[irq_event_arch.public_type_definitions]]
[[signal.public_type_definitions]]
[[mutex.public_type_definitions]]

{{#tasks}}
#define TASK_ID_{{name}} {{idx}}
{{/tasks}}

{{#irq_events}}
#define SIGNAL_SET_IRQ_{{name}} {{sig_set}}
{{/irq_events}}

[[irq_event.public_macros]]
[[irq_event_arch.public_macros]]
[[signal.public_macros]]
[[mutex.public_macros]]

void {{prefix}}start(void);

void {{prefix}}yield(void);

void {{prefix}}irq_event_raise(IrqEventId);

[[signal.public_function_definitions]]

[[mutex.public_function_definitions]]

#endif /* RTOS_RIGEL_H */
