#include "chibihash/chibihash.c"

inline internal S64
msi_lookup64(U64 hash, S64 log2_arr_count, S64 last_index)
{
	Assert(0 < log2_arr_count && log2_arr_count <= 63 && 0 <= last_index);
	U64 mask = (1 << log2_arr_count) - 1,
	    step = (hash >> (64 - log2_arr_count)) | 1;
	S64 result = (last_index + (S64)step) & (S64)mask;
	return result;
}
inline internal S32
msi_lookup32(U64 hash, S32 log2_arr_count, S32 last_index)
{
	Assert(0 < log2_arr_count && log2_arr_count <= 31 && 0 <= last_index);
	U32 mask = (1 << log2_arr_count) - 1,
	    step = ((U32)hash >> (64 - log2_arr_count)) | 1;
	S32 result = (last_index + (S32)step) & (S32)mask;
	return result;
}
inline internal S16
msi_lookup16(U64 hash, S16 log2_arr_count, S16 last_index)
{
	Assert(0 < log2_arr_count && log2_arr_count <= 15 && 0 <= last_index);
	U16 mask = (U16)(1 << log2_arr_count) - 1,
	    step = (U16)(hash >> (64 - log2_arr_count)) | 1;
	S16 result = (last_index + step) & mask;
	return result;
}
