/* REVIEW(beau): I believe there's a simple mask check for is_lower or
 * is_upper. If I find good code for them isn't being generated on all
 * platforms then I'll fix it here.
 */

function B32 char_is_space(U8 c)          {return c == ' '  || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';}
function B32 char_is_upper(U8 c)          {return 'A' <= c && c <= 'Z';}
function B32 char_is_lower(U8 c)          {return 'a' <= c && c <= 'z';}
function B32 char_is_alpha(U8 c)          {return char_is_upper(c) || char_is_lower(c);}
function B32 char_is_slash(U8 c)          {return c == '/' || c == '\\';}
// function B32 char_is_digit(U8 c, U32 base){return }
function U8 char_to_lower(U8 c)           {if (char_is_upper(c)) c+=('a'-'A'); return c;}
function U8 char_to_upper(U8 c)           {if (char_is_lower(c)) c-=('a'-'A'); return c;}
function U8 char_to_correct_slash(U8 c)   {if (char_is_slash(c)) c='/'; return c;}

function String8  str8_range(U8 *first, U8 *one_past_last)    {return str8(first, (U64)(one_past_last-first));}
function String16 str16_range(U16 *first, U16 *one_past_last) {return str16(first, (U64)(one_past_last-first));}
function String32 str32_range(U32 *first, U32 *one_past_last) {return str32(first, (U64)(one_past_last-first));}

function String8 push_str8(Arena *arena, U64 count)
{
	return (String8){push_array_no_zero(arena, U8, count), count};
}
function String8 push_str8_nt(Arena *arena, U64 count)
{
	String8 result = {
		push_array_no_zero(arena, U8, count + 1),
		count
	};
	result.str[count] = 0;

	return result;
}
function String8 push_str8_fill_byte(Arena *arena, U64 count, U8 byte)
{
	String8 result = push_str8(arena, count);
	MemorySet(result.str, byte, count);

	return result;
}
function String8 push_str8_fill_byte_nt(Arena *arena, U64 count, U8 byte)
{
	String8 result = push_str8_nt(arena, count);
	MemorySet(result.str, byte, count);

	return result;
}
function String8 push_str8_copy(Arena *arena, String8 src)
{
	String8 result = push_str8_nt(arena, src.count);
	MemoryCopy(result.str, src.str, src.count);

	return result;
}
function String8 push_str8_cat(Arena *arena, String8 s1, String8 s2)
{
	U64 newcount = s1.count + s2.count;
	String8 result = push_str8_nt(arena, newcount);
	MemoryCopy(result.str, s1.str, s1.count);
	MemoryCopy(result.str + s1.count, s2.str, s2.count);
	result.str[result.count] = 0;

	return result;
}



function String8 str8_substr(String8 string, Rng1U64 range)
{
	range.min = ClampTop(range.min, string.count);
	range.max = ClampTop(range.max, string.count);
	return (String8){string.str+range.min, dim_1u64(range)};
}
function String8 str8_prefix(String8 string, U64 count)
{
	return (String8){string.str, ClampTop(count, string.count)};
}
function String8 str8_skip(String8 string, U64 count)
{
	count = ClampTop(count, string.count);
	return (String8){string.str+count, string.count-count};
}
function String8 str8_postfix(String8 string, U64 count)
{
	count = ClampTop(count, string.count);
	return (String8){string.str+string.count-count, count};
}
function String8 str8_chop(String8 string, U64 count)
{
	count = ClampTop(count, string.count);
	return (String8){string.str, string.count-count};
}



function String8 upper_from_str8(Arena *arena, String8 string)
{
	String8 result = push_str8(arena, string.count);

	U8 *dst = result.str, *src = string.str;
	for (U64 i = 0; i < string.count; i++, dst++, src++)
	{
		*dst = char_to_upper(*src);
	}

	return result;
}
function String8 lower_from_str8(Arena *arena, String8 string)
{
	String8 result = push_str8(arena, string.count);

	U8 *dst = result.str, *src = string.str;
	for (U64 i = 0; i < string.count; i++, dst++, src++)
	{
		*dst = char_to_lower(*src);
	}

	return result;
}
