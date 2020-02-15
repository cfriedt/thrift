#include "ber.h"

int berUintEncode(const uintmax_t x, void *buffer, const size_t bufferSize) {
    (void) x;
    (void) buffer;
    (void) bufferSize;
    return -1;
}
int berUintEncodeLength(uintmax_t x) {
    return berUintEncode( x, NULL, 0 );
}
int berUintDecode(const void *buffer, const size_t bufferSize, uintmax_t *x) {
    (void) buffer;
    (void) bufferSize;
    (void) x;
    return -1;
}
int berUintDecodeLength(const void *buffer, const size_t bufferSize) {
    return berUintDecode( buffer, bufferSize, NULL );
}
