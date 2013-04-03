typedef uint{{taskid_size}}_t TaskId;
typedef uint8_t SignalId;
typedef uint{{signalset_size}}_t SignalSet;
typedef SignalSet SignalIdOption;
typedef TaskId IrqEventId;

void {{prefix}}yield_to(TaskId);
void {{prefix}}yield(void);
void {{prefix}}block(void);
void {{prefix}}unblock(TaskId);
void {{prefix}}start(void);
void {{prefix}}irq_event_raise(IrqEventId);

SignalId {{prefix}}signal_wait_set(SignalSet signal_set);
void {{prefix}}signal_send_set(TaskId task_id, SignalId signal_id);

SignalIdOption {{prefix}}signal_poll_set(SignalSet signal_set);
bool {{prefix}}signal_peek_set(SignalSet signal_set);
