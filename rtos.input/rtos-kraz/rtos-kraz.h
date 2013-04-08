typedef uint{{taskid_size}}_t TaskId;
typedef uint8_t SignalId;
typedef uint{{signalset_size}}_t SignalSet;
typedef SignalId SignalIdOption;

#define SIGNAL_ID_NONE ((SignalIdOption) 0xffU)

void {{prefix}}start(void);

void {{prefix}}yield(void);

SignalId {{prefix}}signal_wait_set(SignalSet signal_set);
void {{prefix}}signal_send_set(TaskId task_id, SignalId signal_id);

SignalIdOption {{prefix}}signal_poll_set(SignalSet signal_set);
bool {{prefix}}signal_peek_set(SignalSet signal_set);
