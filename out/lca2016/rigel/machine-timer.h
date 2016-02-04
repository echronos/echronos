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
 */
void machine_timer_start(void);

/**
 * Stop platform-specific timers.
 * Typically, this function only makes sense to be called after having previously called machine_timer_start().
 * However, on some platforms this function may generally stop all hardware based timers which may be useful if, e.g.,
 * the boot loader enables such timers by default.
 */
void machine_timer_stop(void);

/**
 * Handle a timer tick.
 * If the application implements the interrupt handler for the timer tick interrupt, it must call this function to
 * ensure that the timer continues to work as expected.
 * Therefore, this function must only be called in interrupt mode.
 *
 * Not all applications may need to implement the interrupt handler for the timer tick interrupt.
 * In that case, the system configuration may specify this function itself as the timer tick interrupt handler.
 *
 * This function always calls the application interface function application_tick_isr() to notify the application that
 * a timer tick has occurred.
 */
void machine_timer_tick_isr(void);

/**
 * An application interface function called by machine_timer_tick_isr() in interrupt mode to notify the application
 * that a timer tick has occurred.
 *
 * This function must be implemented by the application.
 * The application shall implement all its tick handling functionality in this function, even if it also implements
 * an interrupt handler for the timer tick interrupt.
 */
void application_tick_isr(void);
