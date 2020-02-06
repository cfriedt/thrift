#ifndef DEBUG_H_
#define DEBUG_H_

#if defined( DEBUG )
#include <stdio.h>
#define D(fmt, args...) printf("D: " fmt "\n", ##args)
#else
#define D(fmt, args...)
#endif

#endif /* DEBUG_H_ */
