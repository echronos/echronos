/*
 * eChronos Real-Time Operating System
 * Copyright (c) 2017, Commonwealth Scientific and Industrial Research
 * Organisation (CSIRO) ABN 41 687 119 230.
 *
 * All rights reserved. CSIRO is willing to grant you a licence to the eChronos
 * real-time operating system under the terms of the CSIRO_BSD_MIT license. See
 * the file "LICENSE_CSIRO_BSD_MIT.txt" for details.
 *
 * @TAG(CSIRO_BSD_MIT)
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
