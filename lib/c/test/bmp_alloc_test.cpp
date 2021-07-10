#include <cerrno>
#include <stdalign.h>

#define BOOST_TEST_MODULE bmp_alloc
#include <boost/test/unit_test.hpp>

#include "thrift/c/bmp.h"
#include "thrift/c/thrift.h"

using namespace std;

static constexpr size_t N = 135;

struct foo {
  int (*bar)(void);
  uint8_t x;
  int64_t y;
  int16_t z;
};

template <typename T>
static inline size_t align_of(const T* x) {
  size_t i;
  uintptr_t y = (uintptr_t)x;
  constexpr size_t N = sizeof(void*) * 8;

  for (i = 0; i < N; ++i, y >>= 1) {
    if ((y & 1) == 1) {
      break;
    }
  }

  return 1ULL << i;
}

DEFINE_BMP_ALLOC(foo_factory, foo, N, static);

BOOST_AUTO_TEST_CASE(StaticAllocator) {
  BOOST_REQUIRE(align_of(&foo_factory_array[0]) >= alignof(foo));
  BOOST_REQUIRE(align_of(&foo_factory_array[1]) >= alignof(foo));
  BOOST_REQUIRE(sizeof(foo_factory_array) >= N * sizeof(foo));
  BOOST_REQUIRE(sizeof(foo_factory_array) == foo_factory.mem_size);
  BOOST_REQUIRE(sizeof(foo_factory_array[0]) == foo_factory.block_size);
  BOOST_REQUIRE(sizeof(foo_factory_bmp) * 8 >= N);

  for (size_t i = 0, M = BITS_TO_LONGS(N); i < M; ++i) {
    if (i < M - 1) {
      BOOST_REQUIRE_EQUAL(BITS(BITS_PER_LONG), foo_factory_bmp[i]);
      continue;
    }

    if (N % BITS_PER_LONG == 0) {
      BOOST_REQUIRE_EQUAL(BITS(BITS_PER_LONG), foo_factory_bmp[i]);
      continue;
    }

    uintmax_t mask = BITS(N % BITS_PER_LONG);
    BOOST_REQUIRE_EQUAL(mask, mask & foo_factory_bmp[i]);
  }
}

BOOST_AUTO_TEST_CASE(BmpAllocInit) {
  struct bmp_alloc bar_factory;
  BOOST_CHECK_EQUAL(-EINVAL, bmp_alloc_init(nullptr, foo_factory.mem, foo_factory.mem_size,
                                            foo_factory.block_size, alignof(foo), foo_factory_bmp,
                                            ARRAY_SIZE(foo_factory_bmp)));
  BOOST_CHECK_EQUAL(-EINVAL, bmp_alloc_init(&bar_factory, nullptr, foo_factory.mem_size,
                                            foo_factory.block_size, alignof(foo), foo_factory_bmp,
                                            ARRAY_SIZE(foo_factory_bmp)));

  // providing less memory just means that the array will be smaller
  BOOST_CHECK_EQUAL(N - 1, bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size - 1,
                                          foo_factory.block_size, alignof(foo), foo_factory_bmp,
                                          ARRAY_SIZE(foo_factory_bmp)));
  // too small is invalid though
  BOOST_CHECK_EQUAL(-EINVAL,
                    bmp_alloc_init(&bar_factory, foo_factory.mem, 0, foo_factory.block_size,
                                   alignof(foo), foo_factory_bmp, ARRAY_SIZE(foo_factory_bmp)));

  // similarly, increasing the block size means that the array will be smaller
  BOOST_CHECK(N > bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                 foo_factory.block_size + 1, alignof(foo), foo_factory_bmp,
                                 ARRAY_SIZE(foo_factory_array)));
  // too small is invalid though
  BOOST_CHECK_EQUAL(-EINVAL,
                    bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size, 0,
                                   alignof(foo), foo_factory_bmp, ARRAY_SIZE(foo_factory_bmp)));

  // check that using unaligned memory will reduce the size of the array
  BOOST_CHECK(N > bmp_alloc_init(&bar_factory, foo_factory.mem + 1, foo_factory.mem_size + 1,
                                 foo_factory.block_size, alignof(foo), foo_factory_bmp,
                                 ARRAY_SIZE(foo_factory_bmp)));
  // internal mem is properly aligned
  BOOST_CHECK_EQUAL(bar_factory.mem, (uint8_t*)ALIGN_UP(foo_factory.mem + 1, alignof(foo)));

  // unaligned is 0 or 1
  BOOST_REQUIRE_EQUAL(N, bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                        foo_factory.block_size, 0, foo_factory_bmp,
                                        ARRAY_SIZE(foo_factory_bmp)));
  // unaligned is 0 or 1
  BOOST_REQUIRE_EQUAL(N, bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                        foo_factory.block_size, 1, foo_factory_bmp,
                                        ARRAY_SIZE(foo_factory_bmp)));
  // only allow power-of-two alignment
  BOOST_REQUIRE_EQUAL(-EINVAL, bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                              foo_factory.block_size, 3, foo_factory_bmp,
                                              ARRAY_SIZE(foo_factory_bmp)));

  // bmp storage cannot be NULL
  BOOST_CHECK_EQUAL(-EINVAL, bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                            foo_factory.block_size, alignof(foo), nullptr,
                                            ARRAY_SIZE(foo_factory_bmp)));

  // bmp storage size cannot be less than necessary
  BOOST_CHECK_EQUAL(-EINVAL,
                    bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                   foo_factory.block_size, alignof(foo), foo_factory_bmp, 1));
  // bmp storage size cannot be 0
  BOOST_CHECK_EQUAL(-EINVAL,
                    bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                   foo_factory.block_size, alignof(foo), foo_factory_bmp, 0));

  // verify the bitmap has the correct contents
  BOOST_REQUIRE_EQUAL(N, bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                        foo_factory.block_size, alignof(foo), foo_factory_bmp,
                                        ARRAY_SIZE(foo_factory_bmp)));

  for (size_t i = 0, M = BITS_TO_LONGS(N); i < M; ++i) {
    if (i < M - 1) {
      BOOST_REQUIRE_EQUAL(BITS(BITS_PER_LONG), foo_factory_bmp[i]);
      continue;
    }

    if (N % BITS_PER_LONG == 0) {
      BOOST_REQUIRE_EQUAL(BITS(BITS_PER_LONG), foo_factory_bmp[i]);
      continue;
    }

    uintmax_t mask = BITS(N % BITS_PER_LONG);
    BOOST_REQUIRE_EQUAL(mask, mask & foo_factory_bmp[i]);
  }
}

BOOST_AUTO_TEST_CASE(BmpAvail) {
  struct bmp_alloc bar_factory;
  const size_t M = BITS_TO_LONGS(N);

  BOOST_REQUIRE_EQUAL(N, bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                        foo_factory.block_size, alignof(foo), foo_factory_bmp,
                                        ARRAY_SIZE(foo_factory_bmp)));

  memset(foo_factory_bmp, 0, sizeof(foo_factory_bmp));
  BOOST_CHECK_EQUAL(0, bmp_avail(&bar_factory));

  foo_factory_bmp[0] = 1;
  BOOST_CHECK_EQUAL(1, bmp_avail(&bar_factory));

  foo_factory_bmp[1] = 1;
  BOOST_CHECK_EQUAL(2, bmp_avail(&bar_factory));

  foo_factory_bmp[0] = BITS(BITS_PER_LONG);
  foo_factory_bmp[1] = 0;
  BOOST_CHECK_EQUAL(BITS_PER_LONG, bmp_avail(&bar_factory));

  for (size_t i = 0, M = BITS_TO_LONGS(N); i < M; ++i) {
    foo_factory_bmp[i] = BITS(BITS_PER_LONG);
  }
  BOOST_CHECK_EQUAL(N, bmp_avail(&bar_factory));
}

BOOST_AUTO_TEST_CASE(BmpAvailContig) {
  struct bmp_alloc bar_factory;
  BOOST_REQUIRE_EQUAL(N, bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                        foo_factory.block_size, alignof(foo), foo_factory_bmp,
                                        ARRAY_SIZE(foo_factory_bmp)));

  memset(foo_factory_bmp, 0, sizeof(foo_factory_bmp));
  BOOST_CHECK_EQUAL(-EINVAL, bmp_avail_contig(&bar_factory, 0));

  foo_factory_bmp[0] = 1;
  BOOST_CHECK_EQUAL(0, bmp_avail_contig(&bar_factory, 1));

  foo_factory_bmp[0] = 2;
  BOOST_CHECK_EQUAL(1, bmp_avail_contig(&bar_factory, 1));

  foo_factory_bmp[0] = BITS(2) << 1;
  BOOST_CHECK_EQUAL(1, bmp_avail_contig(&bar_factory, 2));

  foo_factory_bmp[0] = BITS(BITS_PER_LONG);
  foo_factory_bmp[1] = BITS(BITS_PER_LONG);
  BOOST_CHECK_EQUAL(0, bmp_avail_contig(&bar_factory, 2 * BITS_PER_LONG));

  foo_factory_bmp[0] = BITS(2) << (BITS_PER_LONG - 2);
  foo_factory_bmp[1] = BITS(4);
  BOOST_CHECK_EQUAL((BITS_PER_LONG - 2), bmp_avail_contig(&bar_factory, 6));
}

BOOST_AUTO_TEST_CASE(BmpPtr) {
  BOOST_CHECK_EQUAL(&foo_factory_array[0], bmp_ptr(&foo_factory, 0));
  BOOST_CHECK_EQUAL(&foo_factory_array[1], bmp_ptr(&foo_factory, 1));
  BOOST_CHECK_EQUAL(&foo_factory_array[N - 1], bmp_ptr(&foo_factory, N - 1));
  BOOST_CHECK_EQUAL(nullptr, bmp_ptr(&foo_factory, 42424242));
}

BOOST_AUTO_TEST_CASE(BmpBit) {
  BOOST_CHECK_EQUAL(0, bmp_bit(&foo_factory, &foo_factory_array[0]));
  BOOST_CHECK_EQUAL(1, bmp_bit(&foo_factory, &foo_factory_array[1]));
  BOOST_CHECK_EQUAL(N - 1, bmp_bit(&foo_factory, &foo_factory_array[N - 1]));
  BOOST_CHECK_EQUAL(-1, bmp_bit(&foo_factory, (uint8_t*)(&foo_factory_array[1]) + 1));
  BOOST_CHECK_EQUAL(-1, bmp_bit(&foo_factory, nullptr));
}

BOOST_AUTO_TEST_CASE(BmpAlloc) {
  struct bmp_alloc bar_factory;
  BOOST_REQUIRE_EQUAL(N, bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                        foo_factory.block_size, alignof(foo), foo_factory_bmp,
                                        ARRAY_SIZE(foo_factory_bmp)));

  /*
   * Invalid Parameters / Degenerate Cases
   */

  BOOST_CHECK_EQUAL(nullptr, bmp_alloc(nullptr, N));
  BOOST_CHECK_EQUAL(nullptr, bmp_alloc(&bar_factory, 0));

  /*
   * Some Short-circuit Failures
   * Let
   * n  := the total number of blocks
   * nf := be the total number of free blocks
   * mf := be the max size of contiguous free blocks
   */

  // Test the cases where
  // m > n: there are only N blocks, and we request even more than that
  BOOST_CHECK_EQUAL(nullptr, bmp_alloc(&bar_factory, N + 1));

  // m > nf: e.g. here therare are 4 blocks free, and we request 5
  memset(foo_factory_bmp, 0xff, sizeof(foo_factory_bmp));
  memset(foo_factory_bmp, 0, BITS_TO_LONGS(N) * BYTES_PER_LONG);
  foo_factory_bmp[0] = BITS(4) << 3;
  BOOST_CHECK_EQUAL(nullptr, bmp_alloc(&bar_factory, 5));

  // m > mf: e.g. here there are 8 blocks, and we request 8, but mf is 4
  memset(foo_factory_bmp, 0, sizeof(foo_factory_bmp));
  foo_factory.bmp[0] = (BITS(4) << 25) | (BITS(4) << 2);
  BOOST_CHECK_EQUAL(nullptr, bmp_alloc(&bar_factory, 8));

  /*
   * Happy Path
   */

  memset(foo_factory_bmp, 0xff, sizeof(foo_factory_bmp));

  BOOST_CHECK_EQUAL(&foo_factory_array[0], bmp_alloc(&bar_factory, 1));
  BOOST_CHECK_EQUAL(&foo_factory_array[1], bmp_alloc(&bar_factory, 2));
  BOOST_CHECK_EQUAL(&foo_factory_array[3], bmp_alloc(&bar_factory, 2));

  /*
   * Allocations that wrap around long boundaries
   */

  memset(foo_factory_bmp, 0xff, sizeof(foo_factory_bmp));

  // Mainly 3 cases of concern where alloc should result in success

  // 1) a full long of bits + some additional wraparound
  static_assert(N > BITS_PER_LONG, "");
  memset(&foo_factory_bmp, 0, sizeof(foo_factory_bmp));
  foo_factory_bmp[0] = BITS(BITS_PER_LONG);
  foo_factory_bmp[1] = BITS(2);
  BOOST_CHECK(nullptr != bmp_alloc(&bar_factory, BITS_PER_LONG + 2));

  // 2) partial bits before and after wrap-around
  static_assert(N > BITS_PER_LONG, "");
  memset(&foo_factory_bmp, 0, sizeof(foo_factory_bmp));
  foo_factory_bmp[0] = BITS(BITS_PER_LONG) & ~(BITS(5));
  foo_factory_bmp[1] = BITS(2);
  BOOST_CHECK(nullptr != bmp_alloc(&bar_factory, BITS_PER_LONG - 5 + 2));

  // 3) at least two full longs of bits + some additional wraparound
  static_assert(N > 2 * BITS_PER_LONG, "");
  memset(&foo_factory_bmp, 0, sizeof(foo_factory_bmp));
  foo_factory_bmp[0] = BITS(BITS_PER_LONG);
  foo_factory_bmp[1] = BITS(BITS_PER_LONG);
  foo_factory_bmp[2] = BITS(2);
  BOOST_CHECK(nullptr != bmp_alloc(&bar_factory, 2 * BITS_PER_LONG + 2));
}

BOOST_AUTO_TEST_CASE(BmpFree) {
  struct bmp_alloc bar_factory;
  BOOST_REQUIRE_EQUAL(N, bmp_alloc_init(&bar_factory, foo_factory.mem, foo_factory.mem_size,
                                        foo_factory.block_size, alignof(foo), foo_factory_bmp,
                                        ARRAY_SIZE(foo_factory_bmp)));

  /*
   * Invalid Parameters
   */
  // TODO: add tests to check bitmap is unchanged

  /*
   * Happy Path
   */

  memset(foo_factory_bmp, 0, sizeof(foo_factory_bmp));
  bmp_free(&bar_factory, &foo_factory_array[0], 1);
  BOOST_CHECK_EQUAL(1, foo_factory_bmp[0]);

  memset(foo_factory_bmp, 0, sizeof(foo_factory_bmp));
  bmp_free(&bar_factory, &foo_factory_array[1], 2);
  BOOST_CHECK_EQUAL(6, foo_factory_bmp[0]);

  memset(foo_factory_bmp, 0, sizeof(foo_factory_bmp));
  bmp_free(&bar_factory, &foo_factory_array[0], BITS_PER_LONG);
  BOOST_CHECK_EQUAL(BITS(BITS_PER_LONG), foo_factory_bmp[0]);

  /*
   * Frees that wrap around long boundaries
   */
  memset(foo_factory_bmp, 0, sizeof(foo_factory_bmp));
  bmp_free(&bar_factory, &foo_factory_array[0], 2 * BITS_PER_LONG);
  BOOST_CHECK_EQUAL(BITS(BITS_PER_LONG), foo_factory_bmp[0]);
  BOOST_CHECK_EQUAL(BITS(BITS_PER_LONG), foo_factory_bmp[1]);

  memset(foo_factory_bmp, 0, sizeof(foo_factory_bmp));
  bmp_free(&bar_factory, &foo_factory_array[BITS_PER_LONG / 2], BITS_PER_LONG);
  BOOST_CHECK_EQUAL(BITS(BITS_PER_LONG / 2) << (BITS_PER_LONG / 2), foo_factory_bmp[0]);
  BOOST_CHECK_EQUAL(BITS(BITS_PER_LONG / 2), foo_factory_bmp[1]);
}
