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
#ifndef STDINT_H
#define STDINT_H

#define UINT8_C(L) ((uint8_t)(L))
#define UINT8_MAX UINT8_C(0xFFU)

#define UINT32_C(L) ((uint32_t)(L))
#define UINT32_MAX UINT32_C(0xFFFFFFFFU)

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#endif
