#ifndef REVERSEN_H_
#define REVERSEN_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>

/**
 * Reverse a string of bytes
 *
 * @param p a pointer to where the string begins
 * @param N the length of the string
 */
void reverseN(const void *p, const size_t N);

#if defined(__cplusplus)
}
#endif

#endif /* REVERSEN_H_ */
