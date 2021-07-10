#pragma once

/*
 * Bitmap Allocator
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifndef ALIGN_UP
#define ALIGN_UP(val, align) ((((size_t)val) + ((align)-1)) & ~((align)-1))
#endif

#ifndef IS_ALIGNED
#define IS_ALIGNED(ptr, align) (((uintmax_t)(ptr) & (~((uintmax_t)(align)-1))) == (uintmax_t)(ptr))
#endif

#define BYTES_PER_LONG sizeof(uintmax_t)
#define BITS_PER_LONG (sizeof(uintmax_t) * BYTES_PER_LONG)
#define BIT(x) (1ULL << (x))
#define BITS(x) (((x) == BITS_PER_LONG) ? (uintmax_t)-1 : BIT(x) - 1)
#define BITS_TO_LONGS(n)                                                                           \
  (((n) / BITS_PER_LONG == 0)                                                                      \
       ? 1                                                                                         \
       : (((n) % BITS_PER_LONG == 0) ? (n) / BITS_PER_LONG : (n) / BITS_PER_LONG + 1))
#define GET_BIT_AT(l, n) ((l[(n) / BITS_PER_LONG] >> ((n) % BITS_PER_LONG)) & 0x1)
#define SET_BIT_AT(l, n)                                                                           \
  do {                                                                                             \
    l[(n) / BITS_PER_LONG] |= (1ULL << ((n) % BITS_PER_LONG));                                     \
  } while (0)
#define CLEAR_BIT_AT(l, n)                                                                         \
  do {                                                                                             \
    l[(n) / BITS_PER_LONG] &= ~(1ULL << ((n) % BITS_PER_LONG));                                    \
  } while (0)

#ifdef __cplusplus
extern "C" {
#endif

struct bmp_alloc {
  uint8_t* mem;
  size_t mem_size;
  size_t block_size;
  size_t block_align;
  uintmax_t* bmp;
};

#define DEFINE_BMP_ALLOC(sym, type, n, visibility)                                                 \
  visibility type sym##_array[n];                                                                  \
  visibility uintmax_t sym##_bmp[BITS_TO_LONGS(n)]                                                 \
      = {[0 ...(BITS_TO_LONGS(n) - 1)] = (uintmax_t)-1};                                           \
  visibility struct bmp_alloc sym {                                                                \
    .mem = (uint8_t*)sym##_array, .mem_size = sizeof(sym##_array),                                 \
    .block_size = sizeof(sym##_array[0]), .block_align = alignof(type), .bmp = sym##_bmp,          \
  }

int bmp_alloc_init(struct bmp_alloc* inst,
                   uint8_t* mem,
                   size_t mem_size,
                   size_t block_size,
                   size_t block_align,
                   uintmax_t* bmp,
                   size_t bmp_size);
size_t bmp_avail(const struct bmp_alloc* inst);
ssize_t bmp_avail_contig(const struct bmp_alloc* inst, size_t bits);
void* bmp_ptr(const struct bmp_alloc* inst, size_t bit);
ssize_t bmp_bit(const struct bmp_alloc* inst, void* ptr);
void* bmp_alloc(struct bmp_alloc* inst, size_t m);
void bmp_free(struct bmp_alloc* inst, void* ptr, size_t bits);
void bmp_free_bit(struct bmp_alloc* inst, size_t start_bit, size_t bits);

#ifdef __cplusplus
}
#endif
