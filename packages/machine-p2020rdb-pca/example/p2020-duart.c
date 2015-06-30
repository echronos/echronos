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

#define CCSRBAR_DUART1_OFFSET 0x4500
#define DUART1_REGISTER_BASE (CCSRBAR + CCSRBAR_DUART1_OFFSET)

#define DUART_REGISTER_URBR_UTHR_UDLB_OFFSET 0
#define DUART_REGISTER_UIER_UDMB_OFFSET 1
#define DUART_REGISTER_ULCR_OFFSET 3
#define DUART_REGISTER_ULSR_OFFSET 5

/* DUART registers are byte-width */
#define DUART_URBR1 (volatile uint8_t *)(DUART1_REGISTER_BASE + DUART_REGISTER_URBR_UTHR_UDLB_OFFSET)
#define DUART_UTHR1 DUART_URBR1
#define DUART_UIER1 (volatile uint8_t *)(DUART1_REGISTER_BASE + DUART_REGISTER_UIER_UDMB_OFFSET)
#define DUART_ULCR1 (volatile uint8_t *)(DUART1_REGISTER_BASE + DUART_REGISTER_ULCR_OFFSET)
#define DUART_ULSR1 (volatile uint8_t *)(DUART1_REGISTER_BASE + DUART_REGISTER_ULSR_OFFSET)
/* The Divisor LSB and MSB registers share a location with other registers, but only accessible when ULCR[DLAB]=1 */
#define DUART_UDLB1 DUART_URBR1
#define DUART_UDMB1 DUART_UIER1

/* Bit field masks/values for various DUART registers */
#define ULCR_DLAB_SET 0x80
#define ULCR_DLAB_CLEAR 0x7f
#define ULCR_WLS_5_BITS 0x0
#define ULCR_WLS_6_BITS 0x1
#define ULCR_WLS_7_BITS 0x2
#define ULCR_WLS_8_BITS 0x3
#define UIER_MASK_ALL 0
#define ULSR_THRE_SET 0x20

/* For more, see the Baud Rate Examples table in the P2020 QorIQ Integrated Processor Reference Manual (P2020RM) */
#define DIVISOR_115200_600MHZ_UPPER 0x1
#define DIVISOR_115200_600MHZ_LOWER 0x46

bool
duart1_tx_ready(void)
{
    return !!(*DUART_ULSR1 & ULSR_THRE_SET);
}

void
duart1_tx_put(const char c)
{
    if (!duart1_tx_ready()) {
        /* await(DUART_ULSR[THRE]) */
        while (!duart1_tx_ready());
        /* indicate an overrun */
        *DUART_UTHR1 = '@';
        return;
    }

    *DUART_UTHR1 = (uint8_t)c;
}

void
duart1_init(void)
{
    /* Need to have ULCR[DLAB] = 1 set while modifying UDMB and UDLB */
    *DUART_ULCR1 |= ULCR_DLAB_SET;
    *DUART_UDMB1 = DIVISOR_115200_600MHZ_UPPER;
    *DUART_UDLB1 = DIVISOR_115200_600MHZ_LOWER;
    *DUART_ULCR1 &= ULCR_DLAB_CLEAR;

    /* Set word length ULCR[WLS] = 8 bits */
    *DUART_ULCR1 |= ULCR_WLS_8_BITS;

    /* Disable all interrupts to begin with */
    *DUART_UIER1 = UIER_MASK_ALL;
}
