#ifndef BASE_HASH_H
#define BASE_HASH_H

#include "chibihash/chibihash.h"

// common hashing-related utils

// https://nullprogram.com/blog/2022/08/08/
// NOTE(beau): for first call, last_index is initialized to the hash
inline internal S64 msi_lookup64(U64 hash, S64 log2_arr_count, S64 last_index);
inline internal S32 msi_lookup32(U64 hash, S32 log2_arr_count, S32 last_index);
inline internal S16 msi_lookup16(U64 hash, S16 log2_arr_count, S16 last_index);

#endif // BASE_HASH_H
