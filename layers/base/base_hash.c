#include "chibihash/chibihash.c"

inline function U64
msi_lookup64(U64 hash, U64 log2_arr_count, U64 last_index)
{
	U64 mask = (1 << log2_arr_count) - 1;
	U64 step = (hash >> (64 - log2_arr_count)) | 1;
	U64 result = (last_index + step) & mask;
	return result;
}
inline function U32
msi_lookup32(U64 hash, U32 log2_arr_count, U32 last_index)
{
	U32 mask = (1 << log2_arr_count) - 1;
	U32 step = (U32)((hash >> (64 - log2_arr_count)) | 1);
	U32 result = (last_index + step) & mask;
	return result;
}
inline function U16
msi_lookup16(U64 hash, U16 log2_arr_count, U16 last_index)
{
	U16 mask = (1 << log2_arr_count) - 1;
	U16 step = (U16)((hash >> (64 - log2_arr_count)) | 1);
	U16 result = (last_index + step) & mask;
	return result;
}
