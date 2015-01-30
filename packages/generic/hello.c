/*<module>
  <code_gen>template</code_gen>
  <schema>
   <entry name="phrase" type="string" default="Hello, world!" />
  </schema>
</module>*/

#include <stdint.h>
#include "debug.h"

int
main(void)
{
    debug_println("{{phrase}}");

    for (;;)
    {
    }
}
