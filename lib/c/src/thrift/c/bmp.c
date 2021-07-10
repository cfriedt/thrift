#include <assert.h>
#include <errno.h>
#include <string.h>

#include "bmp.h"

/*
 * Bitmap Allocator
 */

int bmp_alloc_init(struct bmp_alloc* inst,
                   uint8_t* mem,
                   size_t mem_size,
                   size_t block_size,
                   size_t block_align,
                   uintmax_t* bmp,
                   size_t bmp_size) {

  uint8_t* aligned_mem;
  size_t bits, m;

  if (inst == NULL || mem == NULL || bmp == NULL || mem_size == 0 || block_size == 0) {
    return -EINVAL;
  }

  memset(inst, 0, sizeof(*inst));

  if (block_align == 0) {
    block_align = 1;
  }

  if (__builtin_popcountll(block_align) != 1) {
    return -EINVAL;
  }

  // TODO: there may be some weird corner cases surrouding block_align
  // and block_size. E.g. if the alignment is larger than the size.

  aligned_mem = (uint8_t*)ALIGN_UP(mem, block_align);
  mem_size -= (aligned_mem - mem);
  mem = aligned_mem;

  bits = mem_size / block_size;
  if (bmp_size < BITS_TO_LONGS(bits)) {
    return -EINVAL;
  }

  inst->mem = mem;
  inst->mem_size = mem_size;
  inst->block_size = block_size;
  inst->block_align = block_align;
  inst->bmp = bmp;

  memset(bmp, 0xff, BYTES_PER_LONG * BITS_TO_LONGS(bits));
  for (m = BYTES_PER_LONG * BITS_TO_LONGS(bits); m > bits; --m) {
    CLEAR_BIT_AT(bmp, m - 1);
  }

  return bits;
}

size_t bmp_avail(const struct bmp_alloc* x) {
  size_t avail = 0;

  if (x == NULL) {
    return 0;
  }

  const size_t n = x->mem_size / x->block_size;
  assert(n > 0);

  const size_t m = BITS_TO_LONGS(n);

  for (size_t i = 0; i < m; ++i) {
    uintmax_t bmp = x->bmp[i];
    if (i == m - 1) {
      bmp &= BITS((n % BITS_PER_LONG));
    }
    avail += __builtin_popcountll(bmp);
  }

  return avail;
}

// find the first bit where there are at least m consecutive bits
ssize_t bmp_avail_contig(const struct bmp_alloc* inst, size_t m) {
  if (inst == NULL || m == 0) {
    return -EINVAL;
  }

  const size_t n = inst->mem_size / inst->block_size;
  if (m > n) {
    return -E2BIG;
  }

  const size_t nf = bmp_avail(inst);
  if (m > nf) {
    return -ENOMEM;
  }

  for (size_t i = 0, j = 0; i < n - m;) {
    if (GET_BIT_AT(inst->bmp, i + j) == 0) {
      i += j + 1;
      continue;
    }
    ++j;
    if (j == m) {
      return i;
    }
  }

  return -ENOMEM;
}

void* bmp_ptr(const struct bmp_alloc* inst, size_t bit) {
  uint8_t* r;

  if (inst == NULL) {
    return NULL;
  }

  const size_t n = inst->mem_size / inst->block_size;
  assert(n > 0);

  if (bit >= n) {
    return NULL;
  }

  const size_t m = BITS_TO_LONGS(n);
  assert(m > 0);

  r = inst->mem + bit * inst->block_size;

  return (void*)r;
}

ssize_t bmp_bit(const struct bmp_alloc* inst, void* ptr) {
  ptrdiff_t diff;

  if (inst == NULL) {
    return -1;
  }

  const size_t n = inst->mem_size / inst->block_size;
  assert(n > 0);

  if (!IS_ALIGNED(ptr, inst->block_align)) {
    return -1;
  }

  if ((uint8_t*)ptr < inst->mem) {
    return -1;
  }

  diff = (uint8_t*)ptr - inst->mem;
  size_t bit = diff / inst->block_size;

  if (bit >= n) {
    return -1;
  }

  return bit;
}

void* bmp_alloc(struct bmp_alloc* x, size_t m) {

  if (x == NULL || m == 0) {
    return NULL;
  }

  size_t n = x->mem_size / x->block_size;
  assert(n > 0);
  if (m > n) {
    return NULL;
  }

  ssize_t pos = bmp_avail_contig(x, m);
  if (pos < 0) {
    return NULL;
  }

  assert((size_t)pos < n);
  for (size_t i = pos, M = i + m; i < M; ++i) {
    CLEAR_BIT_AT(x->bmp, i);
  }

  return bmp_ptr(x, pos);
}

void bmp_free(struct bmp_alloc* inst, void* ptr, size_t bits) {
  if (inst == NULL || ptr == NULL) {
    return;
  }

  ssize_t bit = bmp_bit(inst, ptr);
  if (bit < 0) {
    return;
  }

  size_t n = inst->mem_size / inst->block_size;
  assert(n > 0);
  if (bit + bits > n) {
    bits = n - bit;
  }

  for (size_t M = bit + bits; (size_t)bit < M; ++bit) {
    SET_BIT_AT(inst->bmp, bit);
  }
}

void bmp_free_bit(struct bmp_alloc* inst, size_t start_bit, size_t bits) {
  bmp_free(inst, bmp_ptr(inst, start_bit), bits);
}
