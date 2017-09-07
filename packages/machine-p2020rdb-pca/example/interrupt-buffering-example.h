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
/* 16 bytes is the size of the DUART FIFOs.
 * But we can pick a totally arbitrary size for the buffer we use to pass bytes to the task. */
#define BUF_CAPACITY 256

void interrupt_buffering_example_init(void);
