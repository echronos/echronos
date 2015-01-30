/*<module>
  <code_gen>cpp</code_gen>
</module>*/

#include <stdio.h>
#include "hello_config.h"

#if !defined(PHRASE)
#define PHRASE "Hello, world!"
#endif

int
main(void)
{
    puts(PHRASE);
    return 0;
}
