#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#include <pthread.h>
#include <stdio.h>
#define D(fmt, args...) printf("D: %s:%d: %s(): " fmt "\n", __FILE__, __LINE__, __func__, ##args)

#define E(fmt, args...)                                                                            \
  fprintf(stderr, "E: %s:%d: %s(): " fmt "\n", __FILE__, __LINE__, __func__, ##args)

// get the address family for a given string respresentation
// "/path/to/foo" => AF_UNIX
// "0.0.0.0" => AF_INET
// "::" => AF_INET6
// "google.com" => AF_INET6 or AF_INET
int thrift_c_addr_family(const char* addr);

#ifdef __cplusplus
}
#endif
