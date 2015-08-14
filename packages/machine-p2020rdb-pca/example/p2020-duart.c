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

#define TX_PUT_OVERRUN_CHAR '@'

#define CCSRBAR_DUART1_OFFSET 0x4500
#define CCSRBAR_DUART2_OFFSET 0x4600

/* The Divisor LSB and MSB registers share a location with other registers, but are only accessible when ULCR[DLAB]=1
 * The transmitter and receiver holding registers are at the same location */
#define DUART_REGISTER_URBR_UTHR_UDLB_OFFSET 0
#define DUART_REGISTER_UIER_UDMB_OFFSET 1
/* The interrupt ID and FIFO control registers are at the same location */
#define DUART_REGISTER_UIIR_UFCR_OFFSET 2
#define DUART_REGISTER_ULCR_OFFSET 3
#define DUART_REGISTER_ULSR_OFFSET 5

/* DUART registers are byte-width */
#define DUART1_REGISTER_BASE (CCSRBAR + CCSRBAR_DUART1_OFFSET)
#define DUART_URBR1 (volatile uint8_t *)(DUART1_REGISTER_BASE + DUART_REGISTER_URBR_UTHR_UDLB_OFFSET)
#define DUART_UTHR1 DUART_URBR1
#define DUART_UIER1 (volatile uint8_t *)(DUART1_REGISTER_BASE + DUART_REGISTER_UIER_UDMB_OFFSET)
#define DUART_ULCR1 (volatile uint8_t *)(DUART1_REGISTER_BASE + DUART_REGISTER_ULCR_OFFSET)
#define DUART_ULSR1 (volatile uint8_t *)(DUART1_REGISTER_BASE + DUART_REGISTER_ULSR_OFFSET)
#define DUART_UDLB1 DUART_URBR1
#define DUART_UDMB1 DUART_UIER1

#define DUART2_REGISTER_BASE (CCSRBAR + CCSRBAR_DUART2_OFFSET)
#define DUART_URBR2 (volatile uint8_t *)(DUART2_REGISTER_BASE + DUART_REGISTER_URBR_UTHR_UDLB_OFFSET)
#define DUART_UTHR2 DUART_URBR2
#define DUART_UIER2 (volatile uint8_t *)(DUART2_REGISTER_BASE + DUART_REGISTER_UIER_UDMB_OFFSET)
#define DUART_UIIR2 (volatile uint8_t *)(DUART2_REGISTER_BASE + DUART_REGISTER_UIIR_UFCR_OFFSET)
#define DUART_UFCR2 DUART_UIIR2
#define DUART_ULCR2 (volatile uint8_t *)(DUART2_REGISTER_BASE + DUART_REGISTER_ULCR_OFFSET)
#define DUART_ULSR2 (volatile uint8_t *)(DUART2_REGISTER_BASE + DUART_REGISTER_ULSR_OFFSET)
#define DUART_UDLB2 DUART_URBR2
#define DUART_UDMB2 DUART_UIER2

/* Bit field masks/values for various DUART registers */
#define ULCR_DLAB_SET 0x80
#define ULCR_DLAB_CLEAR 0x7f
#define ULCR_WLS_5_BITS 0x0
#define ULCR_WLS_6_BITS 0x1
#define ULCR_WLS_7_BITS 0x2
#define ULCR_WLS_8_BITS 0x3
#define UIER_MASK_ALL 0
#define UIER_ETHREI_SET 0x2
#define UIER_ERDAI_SET 0x1
#define ULSR_THRE_SET 0x20
#define ULSR_OE_SET 0x2
#define ULSR_DR_SET 0x1
#define UFCR_FEN_SET 0x1
#define UFCR_RFR_SET 0x2
#define UFCR_RTL_1_BYTES 0x0
#define UFCR_RTL_4_BYTES 0x4
#define UFCR_RTL_8_BYTES 0x8
#define UFCR_RTL_14_BYTES 0xc

/* For more, see the Baud Rate Examples table in the P2020 QorIQ Integrated Processor Reference Manual (P2020RM) */
#define DIVISOR_115200_600MHZ_UPPER 0x1
#define DIVISOR_115200_600MHZ_LOWER 0x46

uint8_t
duart2_iid_get(void)
{
    return *DUART_UIIR2 & DUART_IID_BITMASK;
}

void
duart2_tx_interrupt_init(void)
{
    /* Enable transmitter holding register empty interrupt (ETHREI) */
    *DUART_UIER2 |= UIER_ETHREI_SET;
}

void
duart2_rx_interrupt_init(void)
{
    /* Enable received data available interrupt (ERDAI) */
    *DUART_UIER2 |= UIER_ERDAI_SET;
}

void
duart2_init(void)
{
    /* Need to have ULCR[DLAB] = 1 set while modifying UDMB and UDLB */
    *DUART_ULCR2 |= ULCR_DLAB_SET;
    *DUART_UDMB2 = DIVISOR_115200_600MHZ_UPPER;
    *DUART_UDLB2 = DIVISOR_115200_600MHZ_LOWER;
    *DUART_ULCR2 &= ULCR_DLAB_CLEAR;

    /* Set word length ULCR[WLS] = 8 bits */
    *DUART_ULCR2 |= ULCR_WLS_8_BITS;

    /* Disable all interrupts to begin with */
    *DUART_UIER2 = UIER_MASK_ALL;

    /* Enable FIFOs with receiver trigger level 14 bytes: UFCR[RTL] = 11, UFCR[FEN] = 1, 0 for no action other fields */
    *DUART_UFCR2 = UFCR_RTL_14_BYTES | UFCR_FEN_SET;
}

void
duart2_rx_fifo_reset(void)
{
    /* Dump remaining contents of RX FIFO: Set UFCR[RFR] preserving other bits so FIFOs remain enabled */
    *DUART_UFCR2 |= UFCR_RFR_SET;
}

bool
duart2_rx_overrun(void)
{
    return !!(*DUART_ULSR2 & ULSR_OE_SET);
}

bool
duart2_rx_ready(void)
{
    /* RX FIFO has data ready: DUART_ULSR[DR] */
    return !!(*DUART_ULSR2 & ULSR_DR_SET);
}

char
duart2_rx_get(void)
{
    return *DUART_URBR2;
}

bool
duart2_tx_ready(void)
{
    return !!(*DUART_ULSR2 & ULSR_THRE_SET);
}

void
duart2_tx_put(const char c)
{
    if (!duart2_tx_ready()) {
        /* await(DUART_ULSR[THRE]) */
        while (!duart2_tx_ready());
        /* indicate an overrun */
        *DUART_UTHR2 = TX_PUT_OVERRUN_CHAR;
        return;
    }

    *DUART_UTHR2 = (uint8_t)c;
}

void
duart2_interrupt_disable(void)
{
    *DUART_UIER2 = UIER_MASK_ALL;
}

bool
duart1_rx_ready(void)
{
    /* RX FIFO has data ready: DUART_ULSR[DR] */
    return !!(*DUART_ULSR1 & ULSR_DR_SET);
}

char
duart1_rx_get(void)
{
    return *DUART_URBR1;
}

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
        *DUART_UTHR1 = TX_PUT_OVERRUN_CHAR;
        return;
    }

    *DUART_UTHR1 = (uint8_t)c;
}

void
duart1_rx_interrupt_init(void)
{
    /* Enable received data available interrupt (ERDAI) only */
    *DUART_UIER1 |= UIER_ERDAI_SET;
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
