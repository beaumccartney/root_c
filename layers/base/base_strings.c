#if !BUILD_SUPPLEMENTARY_UNIT
	#if COMPILER_MSVC
		#pragma warning (push, 0)
        #elif COMPILER_CLANG
		#pragma clang diagnostic ignored "-Weverything"
	#endif

	#define STB_SPRINTF_IMPLEMENTATION
	#define STB_SPRINTF_STATIC
	#include "stb/stb_sprintf.h"

	#if COMPILER_MSVC
		#pragma warning (pop)
        #elif COMPILER_CLANG
		#pragma clang diagnostic pop
	#endif

#endif // !BUILD_SUPPLEMENTARY_UNIT

/* REVIEW(beau): I believe there's a simple mask check for is_lower or
 * is_upper. If I find good code for them isn't being generated on all
 * platforms then I'll fix it here.
 */

// NOTE(beau): string-int lookup tables, from https://github.com/EpicGamesExt/raddebugger/blob/274b710329e75db819b41a4de841f4171ba9d74c/src/base/base_strings.c#L13-L50

read_only global U8 integer_symbols[16] = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',
};

// NOTE(allen): Includes reverses for uppercase and lowercase hex.
read_only global U8 integer_symbol_reverse[128] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

read_only global U8 base64[64] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '_', '$',
};

read_only global U8 base64_reverse[128] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
  0xFF,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,
  0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0xFF,0xFF,0xFF,0xFF,0x3E,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
  0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0xFF,0xFF,0xFF,0xFF,0xFF,
};

function B32 char_is_space(U8 c)           {return c == ' '  || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';}
function B32 char_is_upper(U8 c)           {return 'A' <= c && c <= 'Z';}
function B32 char_is_lower(U8 c)           {return 'a' <= c && c <= 'z';}
function B32 char_is_alpha(U8 c)           {return char_is_upper(c) || char_is_lower(c);}
function B32 char_is_slash(U8 c)           {return c == '/' || c == '\\';}
function B32 char_is_digit(U8 c, U32 base) {return 0 < base && base <= 16 && integer_symbol_reverse[c] < base;}
function U8 char_to_lower(U8 c)            {if (char_is_upper(c)) c+=('a'-'A'); return c;}
function U8 char_to_upper(U8 c)            {if (char_is_lower(c)) c-=('a'-'A'); return c;}
function U8 char_to_correct_slash(U8 c)    {if (char_is_slash(c)) c='/'; return c;}

function U64 cstring8_length(U8 *c)
{
	U8 *end = c;
	for (; *end != 0; end++);
	return (U64)(end - c);
}
function U64 cstring16_length(U16 *c)
{
	U16 *end = c;
	for (; *end != 0; end++);
	return (U64)(end - c);
}
function U64 cstring32_length(U32 *c)
{
	U32 *end = c;
	for (; *end != 0; end++);
	return (U64)(end - c);
}

function String8  str8_region(U8 *first, U8 *one_past_last)    {return str8(first, (U64)(one_past_last-first));}
function String16 str16_region(U16 *first, U16 *one_past_last) {return str16(first, (U64)(one_past_last-first));}
function String32 str32_region(U32 *first, U32 *one_past_last) {return str32(first, (U64)(one_past_last-first));}

function String8 push_str8(Arena *arena, U64 count)
{
	return (String8){push_array_no_zero(arena, U8, count), count};
}
function String8 push_str8_nt(Arena *arena, U64 count_minus_one)
{
	String8 result = {
		push_array_no_zero(arena, U8, count_minus_one + 1),
		count_minus_one
	};
	result.str[count_minus_one] = 0;

	return result;
}
function String8 push_str8_fill_byte(Arena *arena, U64 count, U8 byte)
{
	String8 result = push_str8(arena, count);
	MemorySet(result.str, byte, count);

	return result;
}
function String8 push_str8_fill_byte_nt(Arena *arena, U64 count_minus_one, U8 byte)
{
	String8 result = push_str8_nt(arena, count_minus_one);
	MemorySet(result.str, byte, count_minus_one);

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
function String8 push_str8fv(Arena *arena, char *fmt, va_list args)
{
	va_list args2;
	va_copy(args2, args);
	U32 needed_bytes = stbsp_vsnprintf(0, 0, fmt, args) + 1;
	String8 result = push_str8(arena, needed_bytes);
	result.count = stbsp_vsnprintf((char*)result.str, needed_bytes, fmt, args2);
	result.str[result.count] = 0;
	va_end(args2);
	return(result);
}
function String8 push_str8f(Arena *arena, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	String8 result = push_str8fv(arena, fmt, args);
	va_end(args);
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
function String8 str8_postfix(String8 string, U64 count)
{
	count = ClampTop(count, string.count);
	return (String8){string.str+string.count-count, count};
}
function String8 str8_skip(String8 string, U64 count)
{
	count = ClampTop(count, string.count);
	return (String8){string.str+count, string.count-count};
}
function String8 str8_chop(String8 string, U64 count)
{
	count = ClampTop(count, string.count);
	return (String8){string.str, string.count-count};
}
function String8 str8_trim_whitespace(String8 string)
{
	U8 *head = string.str, *tail = string.str + string.count - 1;
	for(; char_is_space(*head); head++);
	for(; char_is_space(*tail); tail--);

	String8 result = str8_region(head, tail + 1);
	return result;
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

function B32 str8_match(String8 a, String8 b, StringMatchFlags flags)
{
	B32 result = 0;
	if (a.count == b.count && flags == 0)
	{
		result = MemoryMatch(a.str, b.str, a.count);
	}
	else if (a.count == b.count || flags & StringMatchFlag_RightSideSloppy)
	{
		U64 count = Min(a.count, b.count);
		result = 1;
		for (U64 i = 0; i < count; i++)
		{
			U8 ac = a.str[i], bc = b.str[i];
			if (flags & StringMatchFlag_CaseInsensitive)
			{
				ac = char_to_lower(ac);
				bc = char_to_lower(bc);
			}
			if (ac != bc)
			{
				result = 0;
				break;
			}
		}
	}

	return result;
}
function U64 str8_find_needle(String8 haystack, U64 start_pos, String8 needle, StringMatchFlags flags)
{
	U64 result = haystack.count;
	if (needle.count > 0)
	{
		S64 end_ix = haystack.count + 1 - needle.count;
		for (S64 ix = start_pos; ix < end_ix; ix++)
		{
			if (str8_match(str8(haystack.str + ix, needle.count), needle, flags))
			{
				result = ix;
				break;
			}
		}
	}
	return result;
}
function B32 str8_ends_with(String8 string, String8 end, StringMatchFlags flags)
{
	String8 postfix = str8_postfix(string, end.count);
	return str8_match(postfix, end, flags);
}
