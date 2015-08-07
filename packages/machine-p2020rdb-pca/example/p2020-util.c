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
 *   ("NICTA") or its licensors is granted. Modified versions of the Program
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

#include <stdint.h>
#include <p2020-util.h>
#include "p2020-duart.h"

#define CCSRBAR_PIC_OFFSET 0x40000
#define PIC_REGISTER_BASE (CCSRBAR + CCSRBAR_PIC_OFFSET)
#define PIC_REGISTER_GCR_OFFSET 0x1020

/* PIC registers are 32 bits wide */
#define PIC_GCR (volatile uint32_t *)(PIC_REGISTER_BASE + PIC_REGISTER_GCR_OFFSET)
#define GCR_MIXED_MODE_SET 0x20000000

/* Switch the PIC (Programmable Interrupt Controller) on the P2020 to "Mixed mode".
 * By default the PIC is in "Pass-through mode", but that routes the PCI Express 1's IRQA signal directly to the
 * external interrupt line of CPU0, causing a storm of spurious external input interrupts.
 * With the PIC switched to "mixed mode", its default mask settings kick in (where everything is masked).
 * When booting from U-Boot, this is not necessary. */
void
machine_pic_init(void)
{
    *PIC_GCR = GCR_MIXED_MODE_SET;
}

/* This is deliberately a busy-waiting use of DUART1 tx so that it doesn't rely on or generate any IRQs */
static void
duart_putc(const char c)
{
    static int initted = 0;

    while (!duart1_tx_ready());

    if (!initted) {
        duart1_init();
        initted = 1;
    }

    duart1_tx_put(c);
}

void
debug_putc(const char c)
{
    /* Purely to be overly safe about the readability of debugging output, ensure that every CR is preceded by a LF,
     * and every LF is followed by a CR. */
    if (c == '\r' || c == '\n') {
        duart_putc('\n');
        duart_putc('\r');
    } else {
        duart_putc(c);
    }
}
