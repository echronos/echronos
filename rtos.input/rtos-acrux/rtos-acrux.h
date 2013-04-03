typedef uint{{taskid_size}}_t TaskId;
typedef TaskId IrqEventId;

void {{prefix}}yield_to(TaskId);
void {{prefix}}yield(void);
void {{prefix}}block(void);
void {{prefix}}unblock(TaskId);
void {{prefix}}start(void);
void {{prefix}}irq_event_raise(IrqEventId);
