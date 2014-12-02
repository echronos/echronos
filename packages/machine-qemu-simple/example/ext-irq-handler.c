#define HANDLER(num) \
void handler##num(void) { \
    while (1); \
}

HANDLER(0)
HANDLER(1)
HANDLER(2)

HANDLER(237)
HANDLER(238)
HANDLER(239)
