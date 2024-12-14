#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

typedef struct String8 String8;
struct String8
{
	U8 *str;
	U64 count;
};

typedef struct String16 String16;
struct String16
{
	U16 *str;
	U64 count;
};

typedef struct String32 String32;
struct String32
{
	U32 *str;
	U64 count;
};

typedef U32 StringMatchFlags;
enum
{
	StringMatchFlag_CaseInsensitive = (1 << 0),
	StringMatchFlag_RightSideSloppy = (1 << 1),
};

function B32 char_is_space(U8 c);
function B32 char_is_upper(U8 c);
function B32 char_is_lower(U8 c);
function B32 char_is_alpha(U8 c);
function B32 char_is_slash(U8 c);
function B32 char_is_digit(U8 c, U32 base);
function U8 char_to_lower(U8 c);
function U8 char_to_upper(U8 c);
function U8 char_to_correct_slash(U8 c);

function U64 cstring8_length(U8 *c);
function U64 cstring16_length(U16 *c);
function U64 cstring32_length(U32 *c);

#define str8_lit(S) str8((U8*)S, sizeof(S) - 1)
#define str8_varg(S) (int)((S).count), ((S).str)

#define str8_array(S,C) str8((U8*)(S), sizeof(*(S))*(C))
#define str8_array_fixed(S) str8((U8*)(S), sizeof((S)))
#define str8_struct(S) str8((U8*)(S), sizeof(*(S)))

#define str8(str, count) ((String8){(str),(count)})
#define str8_zero str8(0, 0)

#define str16(str, count) ((String16){(str),(count)})
#define str16_zero str16(0, 0)

#define str32(str, count) ((String32){(str),(count)})
#define str32_zero str32(0, 0)

function String8 str8_range(U8* first, U8 *one_past_last);
function String16 str16_range(U16* first, U16 *one_past_last);
function String32 str32_range(U32* first, U32 *one_past_last);

function String8 push_str8(Arena *arena, U64 count);
function String8 push_str8_fill_byte(Arena *arena, U64 count, U8 byte);
function String8 push_str8_copy(Arena *arena, String8 s);
function String8 push_str8_cat(Arena *arena, String8 s1, String8 s2);
function String8 push_str8fv(Arena *arena, char *fmt, va_list args);
function String8 push_str8f(Arena *arena, char *fmt, ...);

// NOTE(beau): these add a 0 byte after the end of the string
function String8 push_str8_nt(Arena *arena, U64 count);
function String8 push_str8_fill_byte_nt(Arena *arena, U64 count, U8 byte);

function String8 str8_substr(String8 string, Rng1U64 range);
function String8 str8_prefix(String8 string, U64 count);
function String8 str8_skip(String8 string, U64 amt);
function String8 str8_postfix(String8 string, U64 count);
function String8 str8_chop(String8 string, U64 amt);

function String8 upper_from_str8(Arena *arena, String8 string);
function String8 lower_from_str8(Arena *arena, String8 string);

function B32 str8_match(String8 a, String8 b, StringMatchFlags flags);
function U64 str8_find_needle(String8 haystack, U64 start_pos, String8 needle, StringMatchFlags flags);
function B32 str8_ends_with(String8 string, String8 end, StringMatchFlags flags);

#endif // BASE_STRINGS_H
