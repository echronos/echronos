#ifndef RTOS_RIGEL_H
#define RTOS_RIGEL_H





#include <stdbool.h>
#include <stdint.h>

#include <stdint.h>

#include <stdint.h>

#include <stdbool.h>
#include <stdint.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdint.h>
#include <stdint.h>






typedef uint8_t RtosSignalSet;
typedef RtosSignalSet RtosSignalId;

typedef uint8_t RtosTimerId;
typedef uint32_t RtosTicksAbsolute;
typedef uint16_t RtosTicksRelative;

typedef uint8_t RtosInterruptEventId;

typedef uint8_t RtosMutexId;

typedef uint8_t RtosMessageQueueId;
typedef uint8_t RtosErrorId;
typedef uint8_t RtosTaskId;


















#define RTOS_REENTRANT 




#define RTOS_SIGNAL_SET_EMPTY ((RtosSignalSet) UINT8_C(0))
#define RTOS_SIGNAL_SET_ALL ((RtosSignalSet) UINT8_MAX)
#define RTOS_SIGNAL_SET_ONE ((RtosSignalSet) UINT8_C(4))
#define RTOS_SIGNAL_ID_ONE ((RtosSignalId) RTOS_SIGNAL_SET_ONE)
#define RTOS_SIGNAL_SET_TWO ((RtosSignalSet) UINT8_C(8))
#define RTOS_SIGNAL_ID_TWO ((RtosSignalId) RTOS_SIGNAL_SET_TWO)
#define RTOS_SIGNAL_SET_THREE ((RtosSignalSet) UINT8_C(2))
#define RTOS_SIGNAL_ID_THREE ((RtosSignalId) RTOS_SIGNAL_SET_THREE)
#define RTOS_SIGNAL_SET_FOUR ((RtosSignalSet) UINT8_C(64))
#define RTOS_SIGNAL_ID_FOUR ((RtosSignalId) RTOS_SIGNAL_SET_FOUR)
#define RTOS_SIGNAL_SET_FIVE ((RtosSignalSet) UINT8_C(1))
#define RTOS_SIGNAL_ID_FIVE ((RtosSignalId) RTOS_SIGNAL_SET_FIVE)
#define RTOS_SIGNAL_SET__TASK_TIMER ((RtosSignalSet) UINT8_C(32))
#define RTOS_SIGNAL_ID__TASK_TIMER ((RtosSignalId) RTOS_SIGNAL_SET__TASK_TIMER)
#define RTOS_SIGNAL_SET__RTOS_UTIL ((RtosSignalSet) UINT8_C(16))
#define RTOS_SIGNAL_ID__RTOS_UTIL ((RtosSignalId) RTOS_SIGNAL_SET__RTOS_UTIL)

#define RTOS_TIMER_ID_ONE ((RtosTimerId) UINT8_C(0))
#define RTOS_TIMER_ID_TWO ((RtosTimerId) UINT8_C(1))
#define RTOS_TIMER_ID__TASK_ONE ((RtosTimerId) UINT8_C(2))
#define RTOS_TIMER_ID__TASK_TWO ((RtosTimerId) UINT8_C(3))
#define RTOS_TIMER_ID__TASK_THREE ((RtosTimerId) UINT8_C(4))
#define RTOS_TIMER_ID__TASK_FOUR ((RtosTimerId) UINT8_C(5))
#define RTOS_TIMER_ID__TASK_FIVE ((RtosTimerId) UINT8_C(6))

#define RTOS_INTERRUPT_EVENT_ID_ONE ((RtosInterruptEventId) UINT8_C(0))
#define RTOS_INTERRUPT_EVENT_ID_TWO ((RtosInterruptEventId) UINT8_C(1))
#define RTOS_INTERRUPT_EVENT_ID_THREE ((RtosInterruptEventId) UINT8_C(2))
#define RTOS_INTERRUPT_EVENT_ID_FOUR ((RtosInterruptEventId) UINT8_C(3))
#define RTOS_INTERRUPT_EVENT_ID_FIVE ((RtosInterruptEventId) UINT8_C(4))

#define RTOS_MUTEX_ID_ZERO ((RtosMutexId) UINT8_C(0))
#define RTOS_MUTEX_ID_MAX ((RtosMutexId) UINT8_C(5 - 1))
#define RTOS_MUTEX_ID_ONE ((RtosMutexId) UINT8_C(0))
#define RTOS_MUTEX_ID_TWO ((RtosMutexId) UINT8_C(1))
#define RTOS_MUTEX_ID_THREE ((RtosMutexId) UINT8_C(2))
#define RTOS_MUTEX_ID_FOUR ((RtosMutexId) UINT8_C(3))
#define RTOS_MUTEX_ID_FIVE ((RtosMutexId) UINT8_C(4))

#define RTOS_MESSAGE_QUEUE_ID_ONE ((RtosMessageQueueId) UINT8_C(0))

#define RTOS_TASK_ID_ZERO ((RtosTaskId) UINT8_C(0))
#define RTOS_TASK_ID_MAX ((RtosTaskId)UINT8_C(5 - 1))
#define RTOS_TASK_ID_ONE ((RtosTaskId) UINT8_C(0))
#define RTOS_TASK_ID_TWO ((RtosTaskId) UINT8_C(1))
#define RTOS_TASK_ID_THREE ((RtosTaskId) UINT8_C(2))
#define RTOS_TASK_ID_FOUR ((RtosTaskId) UINT8_C(3))
#define RTOS_TASK_ID_FIVE ((RtosTaskId) UINT8_C(4))






#define rtos_signal_wait(requested_signal) \
    (void) rtos_signal_wait_set(requested_signal)

#define rtos_signal_poll(requested_signal) \
    (rtos_signal_poll_set(requested_signal) != RTOS_SIGNAL_SET_EMPTY)

#define rtos_signal_peek(requested_signal) \
    (rtos_signal_peek_set(requested_signal) != RTOS_SIGNAL_SET_EMPTY)

#define rtos_signal_send(task_id, signal) \
    rtos_signal_send_set(task_id, signal)


void rtos_interrupt_event_raise(RtosInterruptEventId interrupt_event_id);















/*@unused@*/
extern RtosTicksAbsolute rtos_timer_current_ticks;








#ifdef __cplusplus
extern "C" {
#endif





/*@unused@*/
RtosSignalSet rtos_signal_wait_set(RtosSignalSet requested_signals) RTOS_REENTRANT;
/*@unused@*/
RtosSignalSet rtos_signal_poll_set(RtosSignalSet requested_signals);
/*@unused@*/
RtosSignalSet rtos_signal_peek_set(RtosSignalSet requested_signals);
/*@unused@*/
void rtos_signal_send_set(RtosTaskId task_id, RtosSignalSet signals);
void rtos_timer_tick(void);
/*@unused@*/
void rtos_sleep(RtosTicksRelative ticks) RTOS_REENTRANT;
/*@unused@*/
void rtos_timer_enable(RtosTimerId timer_id);
/*@unused@*/
void rtos_timer_disable(RtosTimerId timer_id);
/*@unused@*/
void rtos_timer_oneshot(RtosTimerId timer_id, RtosTicksRelative timeout);
/*@unused@*/
bool rtos_timer_check_overflow(RtosTimerId timer_id);
/*@unused@*/
RtosTicksRelative rtos_timer_remaining(RtosTimerId timer_id);
/*@unused@*/
void rtos_timer_reload_set(RtosTimerId timer_id, RtosTicksRelative reload);
/*@unused@*/
void rtos_timer_error_set(RtosTimerId timer_id, RtosErrorId error_id);
/*@unused@*/
void rtos_timer_signal_set(RtosTimerId timer_id, RtosTaskId task_id, RtosSignalSet signal_set);


void rtos_interrupt_event_task_set(RtosInterruptEventId interrupt_event_id, RtosTaskId task_id);

/*@unused@*/
void rtos_mutex_lock(RtosMutexId) RTOS_REENTRANT;
/*@unused@*/
bool rtos_mutex_try_lock(RtosMutexId);
/*@unused@*/
void rtos_mutex_unlock(RtosMutexId);
/*@unused@*/
bool rtos_mutex_holder_is_current(RtosMutexId);
/*@unused@*/
void rtos_message_queue_put(RtosMessageQueueId message_queue, const void *message)
        RTOS_REENTRANT;
/*@unused@*/
bool rtos_message_queue_try_put(RtosMessageQueueId message_queue, const void *message);
/*@unused@*/
bool rtos_message_queue_put_timeout(RtosMessageQueueId message_queue, const void *message,
                                              RtosTicksRelative timeout) RTOS_REENTRANT;
/*@unused@*/
void rtos_message_queue_get(RtosMessageQueueId message_queue, void *message)
        RTOS_REENTRANT;
/*@unused@*/
bool rtos_message_queue_try_get(RtosMessageQueueId message_queue, void *message);
/*@unused@*/
bool rtos_message_queue_get_timeout(RtosMessageQueueId message_queue, void *message,
                                              RtosTicksRelative timeout) RTOS_REENTRANT;


/*@unused@*/
RtosTaskId rtos_task_current(void);
void rtos_start(void);
/*@unused@*/
void rtos_yield(void) RTOS_REENTRANT;
/*@unused@*/
void rtos_task_start(RtosTaskId task);
#ifdef __cplusplus
}
#endif

#endif /* RTOS_RIGEL_H */