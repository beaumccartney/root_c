#ifndef CHIBIHASH64__HGUARD
#define CHIBIHASH64__HGUARD
#include <stddef.h>
// NOTE(beau): Attribution below. I factored this into separate header and
// c files to not have everything inlined.

// small, fast 64 bit hash function (version 2).
//
// https://github.com/N-R-K/ChibiHash
//
// This is free and unencumbered software released into the public domain.
// For more information, please refer to <https://unlicense.org/>

static uint64_t chibihash64__load32le(const uint8_t *p);
static uint64_t chibihash64__load64le(const uint8_t *p);
static uint64_t chibihash64__rotl(uint64_t x, int n);

static uint64_t chibihash64(const void *keyIn, ptrdiff_t len, uint64_t seed);

#endif // CHIBIHASH64__HGUARD
