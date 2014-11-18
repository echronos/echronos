/*<module>
  <code_gen>template</code_gen>
  <headers>
      <header path="debug.h" code_gen="template" />
  </headers>
  <schema>
   <entry name="prefix" type="c_ident" default="" />
   <entry name="ll_debug" type="c_ident" default="rtos_internal_" />
  </schema>
</module>*/

#include <stdint.h>
#include "debug.h"

extern void {{ll_debug}}debug_putc(char);

void
{{prefix}}debug_print(const char *msg)
{
    while (*msg != 0)
    {
        {{ll_debug}}debug_putc(*msg);
        msg++;
    }
}


void
{{prefix}}debug_println(const char *const msg)
{
    {{prefix}}debug_print(msg);
    {{ll_debug}}debug_putc('\n');
}

static void
put_hexdigit(const uint8_t val)
{
    char ch;
    if (val < 10)
    {
        ch = '0' + val;
    }
    else
    {
        ch = ('a' - 10) + val;
    }
    {{ll_debug}}debug_putc(ch);
}

void
{{prefix}}debug_printhex32(const uint32_t val)
{
    {{ll_debug}}debug_putc('0');
    {{ll_debug}}debug_putc('x');

    put_hexdigit((val >> 28) & 0xf);
    put_hexdigit((val >> 24) & 0xf);
    put_hexdigit((val >> 20) & 0xf);
    put_hexdigit((val >> 16) & 0xf);
    put_hexdigit((val >> 12) & 0xf);
    put_hexdigit((val >> 8) & 0xf);
    put_hexdigit((val >> 4) & 0xf);
    put_hexdigit((val >> 0) & 0xf);
}
