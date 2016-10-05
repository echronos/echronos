/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that these additional
 * terms apply under section 7:
 *
 *   No right, title or interest in or to any trade mark, service mark, logo
 *   or trade name of of National ICT Australia Limited, ABN 62 102 206 173
 *   "NICTA" or its licensors is granted. Modified versions of the Program
 *   must be plainly marked as such, and must not be distributed using
 *   "eChronos" as a trade mark or product name, or misrepresented as being
 *   the original Program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @TAG(NICTA_AGPL)
 */
/**
 * Start a platform-specific timer (typically some hardware timer) that produces periodic timer ticks at about 100ms
 * intervals.
 *
 * @param application_timer_isr an application-defined function that implements the application-specific part of handling a
 *  tick service routine.
 *  This parameter is necessary because on some target platforms, the machine-timer implementation sets up the ISR
 *  handler internally (as opposed to the application associating the handler with some well-known, timer-related
 *  interrupt vector, for example).
 */
void machine_timer_start(void (*application_timer_isr)(void));

/**
 * Stop platform-specific timers.
 * Typically, this function only makes sense to be called after having previously called machine_timer_start().
 * However, on some platforms this function may generally stop all hardware based timers which may be useful if, e.g.,
 * the boot loader enables such timers by default.
 */
void machine_timer_stop(void);

/**
 * Handle a timer tick inside the platform-specific timer implementation.
 *
 * This function ensures that for every timer tick, the timer implementation continues to work as expected and continues
 * to deliver timer ticks.
 * For some target platforms and timer implementations, this function might be empty.
 * For others, per-tick platform-timer handling may be necessary.
 *
 * The application must call this function from its timer tick ISR if it expects the timer implementation to work as
 * expected.
 * Note that this function must only be called in interrupt mode.
 * Also note that this function only deals with the platform's timer implementation itself.
 * That is to say that this function does not call rtos_timer_tick() or any other RTOS APIs.
 */
void machine_timer_tick_isr(void);
