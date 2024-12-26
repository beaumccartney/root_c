#ifndef BASE_HASH_H
#define BASE_HASH_H

#include "chibihash/chibihash.h"

// common hashing-related utils

// https://nullprogram.com/blog/2022/08/08/
// NOTE(beau): for first call, last_index is initialized to the hash
inline function U64 msi_lookup64(U64 hash, U64 log2_arr_count, U64 last_index);
inline function U32 msi_lookup32(U64 hash, U32 log2_arr_count, U32 last_index);
inline function U16 msi_lookup16(U64 hash, U16 log2_arr_count, U16 last_index);

#endif // BASE_HASH_H
