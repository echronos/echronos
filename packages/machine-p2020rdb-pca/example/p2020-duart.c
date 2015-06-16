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

/* DUART registers are byte-width */
#define DUART1_REGISTER_BASE (CCSRBAR + 0x4500)
#define DUART_URBR1 (volatile uint8_t *)DUART1_REGISTER_BASE
#define DUART_UTHR1 DUART_URBR1
#define DUART_UIER1 (volatile uint8_t *)(DUART1_REGISTER_BASE + 1)
#define DUART_ULCR1 (volatile uint8_t *)(DUART1_REGISTER_BASE + 3)
#define DUART_ULSR1 (volatile uint8_t *)(DUART1_REGISTER_BASE + 5)
/* The Divisor LSB and MSB registers share a location with other registers, but only accessible when ULCR[DLAB]=1 */
#define DUART_UDLB1 DUART_URBR1
#define DUART_UDMB1 DUART_UIER1

bool
duart1_tx_ready(void)
{
    return !!(*DUART_ULSR1 & 0x20);
}

void
duart1_tx_put(const char c)
{
    if (!duart1_tx_ready()) {
        /* await(DUART_ULSR[THRE]) */
        while (!duart1_tx_ready());
        /* overrun */
        *DUART_UTHR1 = '@';
        return;
    }

    *DUART_UTHR1 = (uint8_t)c;
}

void
duart1_init(void)
{
    /* Need to have ULCR[DLAB] = 1 set while modifying UDMB and UDLB */
    *DUART_ULCR1 |= 0x80;
    /*
     * For baud rate of 115200 and platform clock frequency of 600Mhz:
     * Divisor = 0x146
     */
    *DUART_UDMB1 = 0x1;
    *DUART_UDLB1 = 0x46;
    /* Clear ULCR[DLAB] */
    *DUART_ULCR1 &= 0x7f;
    /* Set word length ULCR[WLS] = 8 bits */
    *DUART_ULCR1 |= 0x3;

    /* Disable all interrupts to begin with */
    *DUART_UIER1 = 0;
}
