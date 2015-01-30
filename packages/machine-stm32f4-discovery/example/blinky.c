/* @LICENSE(NICTA) */

/*<module>
  <code_gen>template</code_gen>
  <schema>
   <entry name="delay_amount" type="int" default="10000"/>
  </schema>
</module>*/
#include "led.h"
#define DELAY_AMOUNT {{delay_amount}}

static inline void
delay(void)
{
    volatile int i;
    for (i = 0; i < DELAY_AMOUNT; i++)
    {
    }
}

int
main(void)
{
    unsigned int i;
    led_init();

    for (i = 0; ; i++)
    {
        switch (i % 8)
        {
        case 0:
            led_green_on();
            break;
        case 1:
            led_blue_on();
            break;
        case 2:
            led_red_on();
            break;
        case 3:
            led_orange_on();
            break;
        case 4:
            led_green_off();
            break;
        case 5:
            led_blue_off();
            break;
        case 6:
            led_red_off();
            break;
        case 7:
            led_orange_off();
            break;
        }
        delay();
    }
}
