#ifndef BER_H_
#define BER_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#define BER_MAX_BYTES 9
#define BER_SINGLE_BYTE_MODULUS 0x80
#define BER_LONG_FORM_MASK 0x80

int berUintEncode(const uintmax_t x, void *buffer, const size_t bufferSize);
int berUintEncodeLength(uintmax_t x);
int berUintDecode(const void *buffer, const size_t bufferSize, uintmax_t *x);
int berUintDecodeLength(const void *buffer, const size_t bufferSize);

__END_DECLS

#endif /* BER_H_ */
