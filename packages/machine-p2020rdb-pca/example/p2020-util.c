/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that no right, title
 * or interest in or to any trade mark, service mark, logo or trade name
 * of NICTA or its licensors is granted.
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

#include "p2020-duart.h"

/* This is deliberately a busy-waiting use of DUART1 tx so that it doesn't rely on or generate any IRQs */
void
rtos_internal_debug_putc(const char c)
{
    static int initted = 0;
    static int recursion = 0;

    while (!duart1_tx_ready());

    if (!initted) {
        duart1_init();
        initted = 1;
    }

    /* Purely to be overly safe about the readability of debugging output, ensure every CR is preceded by a LF */
    if (!recursion && c == '\r') {
        recursion = 1;
        rtos_internal_debug_putc('\n');
        recursion = 0;
    }

    duart1_tx_put(c);

    /* For similar reasons, ensure every LF is followed by a CR */
    if (!recursion && c == '\n') {
        recursion = 1;
        rtos_internal_debug_putc('\r');
        recursion = 0;
    }
}
