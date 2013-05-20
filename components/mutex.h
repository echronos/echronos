#ifndef _MUTEX_H
#define _MUTEX_H

#include "stdbool.h"

#define MUTEX_TIMEOUT_INFINITE 0xFFFF
#define MUTEX_TIMEOUT_INSTANT 0x0000

typedef struct mutex {{prefix}}mutex;

/**
 * @brief unlock the mutex
 *
 * @param mutex the mutex you wish to unlock
 *
 * @remark if the mutex is currently unlocked, this function has no effect
 * @remark other tasks waiting on this mutex will be scheduled normally. The first one to become
 *         active will take the lock.
 */
#define {{prefix}}mutex_unlock(mutex) { (({{prefix}}mutex *)mutex)->locked = false; }

/**
 * @brief Lock the mutex with no timeout
 *
 * @param mutex the mutex you wish to lock
 */
#define {{prefix}}mutex_lock(mutex) _mutex_lock(mutex, MUTEX_TIMEOUT_INFINITE
[[#has_signal]]
            0,0  \
[[/has_signal]]
)

/**
 * @brief Try to lock the mutex. If it is already locked, return straight away.
 *
 * @param mutex the mutex you wish to lock
 *
 * @return true if the lock was taken, false if the lock was not obtained
 */
#define {{prefix}}mutex_tryLock(mutex) _mutex_lock(mutex, MUTEX_TIMEOUT_INSTANT \
[[#has_signal]]
            0,0  \
[[/has_signal]]
)

/**
 * @brief Try to lock the mutex. If it is already locked, wait for at most
 *        \code timeout
 *        milliseconds and then return
 *
 * @param mutex   the mutex you wish to lock
 * @param timeout the number of milliseconds to wait for the lock
 *
 * @return true if the lock was taken, false if the lock was not obtained
 */
#define {{prefix}}mutex_lockTimeout(mutex, timeout) _mutex_lock(mutex, timeout \
[[#has_signal]]
            0,0  \
[[/has_signal]]
)

[[#has_signal]]
/**
 * @brief Try to lock the mutex. If it is already locked, wait for at most
 *        \code timeout
 *        milliseconds and then raise a signal for a particular task
 *
 * @param mutex   the mutex you wish to lock
 * @param timeout the number of milliseconds to wait for the lock
 * @param signal  the signal to be raised on timeout
 * @param taskToSignal the task that is to be signalled of a timeout
 *
 * @return true if the lock was taken, false if the lock was not obtained
 */
#define {{prefix}}mutex_lockOrSignal(mutex, timeout, signal, taskToSignal) \
    _mutex_lock(mutex, timeout, signal, taskToSignal)
[[/has_signal]]


bool
_mutex_lock({{prefix}}mutex *mutex, uint16_t timeout
[[#has_signal]]
, SignalSet signal, TaskId taskToSignal
[[/has_signal]]
);
