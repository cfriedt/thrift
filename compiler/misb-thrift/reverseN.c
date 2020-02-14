#include <stddef.h>
#include <stdint.h>

#include "reverseN.h"

void reverseN(const void *p, const size_t N)
{
    if (NULL == p || 0 == N) {
        return;
    }

    uint8_t *a, *b, c;

    for (a = (uint8_t *) p, b = a + N - 1; a < b; a++, b--) {
        c  = *b;
        *b = *a;
        *a = c;
    }
}
