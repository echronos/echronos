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
#include <stdbool.h>

#define DUART_IID_BITMASK 0xf
#define DUART_IID_NONE 0x1
/* character time-out */
#define DUART_IID_CTO 0xc
/* received data available */
#define DUART_IID_RDA 0x4
/* transmitter holding register empty */
#define DUART_IID_THRE 0x2

/* DUART2 */
uint8_t duart2_iid_get(void);
void duart2_init(void);

void duart2_rx_interrupt_init(void);
bool duart2_rx_ready(void);
void duart2_rx_fifo_reset(void);
bool duart2_rx_overrun(void);
void duart2_rx_init(void);
char duart2_rx_get(void);

void duart2_tx_interrupt_init(void);
bool duart2_tx_ready(void);
void duart2_tx_put(char c);

void duart2_interrupt_disable(void);

/* DUART1 */
void duart1_init(void);

void duart1_rx_interrupt_init(void);
bool duart1_rx_ready(void);
char duart1_rx_get(void);

bool duart1_tx_ready(void);
/* The tx_put() function should only be used if tx_ready() returns true.
 * If ever called when the tx is not ready, tx_put() will spin until the tx becomes ready, then transmit the '@'
 * character to indicate improper usage. */
void duart1_tx_put(char c);
