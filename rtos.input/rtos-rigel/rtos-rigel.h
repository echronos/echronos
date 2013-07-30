#ifndef RTOS_RIGEL_H
#define RTOS_RIGEL_H

#include <stdint.h>
#include <stdbool.h>

typedef uint{{taskid_size}}_t TaskId;
typedef uint8_t SignalId;
typedef uint{{signalset_size}}_t SignalSet;
typedef SignalId SignalIdOption;
typedef uint{{irqeventid_size}}_t IrqEventId;
typedef uint8_t MutexId;

#define SIGNAL_ID_NONE ((SignalIdOption) 0xffU)

void {{prefix}}start(void);

void {{prefix}}yield(void);

{{#tasks}}
#define TASK_{{name}} {{idx}}
{{/tasks}}

{{#irq_events}}
#define IRQ_EVENT_{{name}} {{idx}}
{{/irq_events}}

{{#irq_events}}
#define IRQ_SIGNAL_SET_{{name}} {{sig_set}}
{{/irq_events}}

void {{prefix}}irq_event_raise(IrqEventId);

SignalId {{prefix}}signal_wait_set(SignalSet signal_set);
void {{prefix}}signal_send_set(TaskId task_id, SignalId signal_id);

SignalIdOption {{prefix}}signal_poll_set(SignalSet signal_set);
bool {{prefix}}signal_peek_set(SignalSet signal_set);

void {{prefix}}mutex_lock(MutexId);
bool {{prefix}}mutex_try_lock(MutexId);
void {{prefix}}mutex_unlock(MutexId);

#endif /* RTOS_RIGEL_H */
