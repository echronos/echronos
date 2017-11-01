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

#ifndef BITBAND_H
#define BITBAND_H

#define BITBAND_VAR(TYPE, NAME) \
    TYPE NAME __attribute__ ((section (".data.bitband"))); \
    extern uint32_t NAME##_bitband[sizeof NAME]

#define BITBAND_VAR_ARRAY(TYPE, NAME, NELEM) \
    TYPE NAME[NELEM] __attribute__ ((section (".data.bitband"))); \
    extern uint32_t NAME##_bitband[sizeof NAME]

#define VOLATILE_BITBAND_VAR(TYPE, NAME) \
    volatile TYPE NAME __attribute__ ((section (".data.bitband"))); \
    extern volatile uint32_t NAME##_bitband[sizeof NAME]

#define VOLATILE_BITBAND_VAR_ARRAY(TYPE, NAME, NELEM) \
    volatile TYPE NAME[NELEM] __attribute__ ((section (".data.bitband"))); \
    extern volatile uint32_t NAME##_bitband[sizeof NAME]


#endif

