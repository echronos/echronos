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

