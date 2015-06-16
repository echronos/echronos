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

#include <stdint.h>
#include <p2020-util.h>
#include "p2020-duart.h"

/* PIC registers are 32 bits wide */
#define PIC_REGISTER_BASE (CCSRBAR + 0x40000)
#define PIC_GCR (volatile uint32_t *)(PIC_REGISTER_BASE + 0x1020)

/* Switch the PIC (Programmable Interrupt Controller) on the P2020 to "Mixed mode".
 * By default the PIC is in "Pass-through mode", but that routes the PCI Express 1's IRQA signal directly to the
 * external interrupt line of CPU0, causing a storm of spurious external input interrupts.
 * With the PIC switched to "mixed mode", its default mask settings kick in (where everything is masked).
 * When booting from U-Boot, this is not necessary. */
void
machine_pic_init(void)
{
    *PIC_GCR = 0x20000000;
}

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
