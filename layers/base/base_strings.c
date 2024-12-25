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
function U8 char_to_forward_slash(U8 c)    {if (char_is_slash(c)) c='/'; return c;}
function U8 char_to_back_slash(U8 c)       {if (char_is_slash(c)) c='\\'; return c;}

function U64 cstring8_length(char *c)
{
	char *end = c;
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

function String8 str8_cstring(char *cstr)
{
	String8 result = {(U8*)cstr, cstring8_length(cstr)};
	return result;
}
function String16 str16_cstring(U16 *cstr)
{
	String16 result = {cstr, cstring16_length(cstr)};
	return result;
}
function String32 str32_cstring(U32 *cstr)
{
	String32 result = {cstr, cstring32_length(cstr)};
	return result;
}

function String8 str8_cstring_capped(char *cstr, U64 cap)
{
	U64 length = 0;
	for (U8 *c = (U8*)cstr; *c != 0 && length < cap; length++, c++);
	String8 result = {(U8*)cstr, length};
	return result;
}
function String16 str16_cstring_capped(U16 *cstr, U64 cap)
{
}
function String32 str32_cstring_capped(U32 *cstr, U64 cap)
{
}

function String8  str8_region(U8 *first, U8 *one_past_last)    {return str8(first, (U64)(one_past_last-first));}
function String16 str16_region(U16 *first, U16 *one_past_last) {return str16(first, (U64)(one_past_last-first));}
function String32 str32_region(U32 *first, U32 *one_past_last) {return str32(first, (U64)(one_past_last-first));}

function String8 push_str8(Arena *arena, U64 length)
{
	return (String8){push_array_no_zero(arena, U8, length), length};
}
function String8 push_str8_nt(Arena *arena, U64 length_minus_one)
{
	String8 result = {
		push_array_no_zero(arena, U8, length_minus_one + 1),
		length_minus_one
	};
	result.buffer[length_minus_one] = 0;

	return result;
}
function String8 push_str8_fill_byte(Arena *arena, U64 length, U8 byte)
{
	String8 result = push_str8(arena, length);
	MemorySet(result.buffer, byte, length);

	return result;
}
function String8 push_str8_fill_byte_nt(Arena *arena, U64 length_minus_one, U8 byte)
{
	String8 result = push_str8_nt(arena, length_minus_one);
	MemorySet(result.buffer, byte, length_minus_one);

	return result;
}
function String8 push_str8_copy(Arena *arena, String8 src)
{
	String8 result = push_str8_nt(arena, src.length);
	MemoryCopy(result.buffer, src.buffer, src.length);

	return result;
}
function String8 push_str8_cat(Arena *arena, String8 s1, String8 s2)
{
	U64 newlength = s1.length + s2.length;
	String8 result = push_str8_nt(arena, newlength);
	MemoryCopy(result.buffer, s1.buffer, s1.length);
	MemoryCopy(result.buffer + s1.length, s2.buffer, s2.length);
	result.buffer[result.length] = 0;

	return result;
}
function String8 push_str8fv(Arena *arena, char *fmt, va_list args)
{
	va_list args2;
	va_copy(args2, args);
	U32 needed_bytes = stbsp_vsnprintf(0, 0, fmt, args) + 1;
	String8 result = push_str8(arena, needed_bytes);
	result.length = stbsp_vsnprintf((char*)result.buffer, needed_bytes, fmt, args2);
	result.buffer[result.length] = 0;
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
	range.min = ClampTop(range.min, string.length);
	range.max = ClampTop(range.max, string.length);
	return (String8){string.buffer+range.min, dim_1u64(range)};
}
function String8 str8_prefix(String8 string, U64 length)
{
	return (String8){string.buffer, ClampTop(length, string.length)};
}
function String8 str8_postfix(String8 string, U64 length)
{
	length = ClampTop(length, string.length);
	return (String8){string.buffer+string.length-length, length};
}
function String8 str8_skip(String8 string, U64 length)
{
	length = ClampTop(length, string.length);
	return (String8){string.buffer+length, string.length-length};
}
function String8 str8_chop(String8 string, U64 length)
{
	length = ClampTop(length, string.length);
	return (String8){string.buffer, string.length-length};
}
function String8 str8_trim_whitespace(String8 string)
{
	U8 *head = string.buffer, *tail = string.buffer + string.length - 1;
	for (; char_is_space(*head); head++);
	for (; char_is_space(*tail); tail--);

	String8 result = str8_region(head, tail + 1);
	return result;
}


function String8 upper_from_str8(Arena *arena, String8 string)
{
	String8 result = push_str8(arena, string.length);

	U8 *dst = result.buffer, *src = string.buffer;
	for (U64 i = 0; i < string.length; i++, dst++, src++)
	{
		*dst = char_to_upper(*src);
	}

	return result;
}
function String8 lower_from_str8(Arena *arena, String8 string)
{
	String8 result = push_str8(arena, string.length);

	U8 *dst = result.buffer, *src = string.buffer;
	for (U64 i = 0; i < string.length; i++, dst++, src++)
	{
		*dst = char_to_lower(*src);
	}

	return result;
}

function B32 str8_match(String8 a, String8 b, StringMatchFlags flags)
{
	B32 result = 0;
	if (a.length == b.length && flags == 0)
	{
		result = MemoryMatch(a.buffer, b.buffer, a.length);
	}
	else if (a.length == b.length || flags & StringMatchFlag_RightSideSloppy)
	{
		U64 count = Min(a.length, b.length);
		result = 1;
		for (U64 i = 0; i < count; i++)
		{
			U8 ac = a.buffer[i], bc = b.buffer[i];
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
	U64 result = haystack.length;
	if (needle.length > 0)
	{
		S64 end_ix = haystack.length + 1 - needle.length;
		for (S64 ix = start_pos; ix < end_ix; ix++)
		{
			if (str8_match(str8(haystack.buffer + ix, needle.length), needle, flags))
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
	String8 postfix = str8_postfix(string, end.length);
	return str8_match(postfix, end, flags);
}



function String8Node* str8_list_push_node(String8List *list, String8Node *node)
{
	SLLQueuePush(list->first, list->last, node);
	list->node_count   += 1;
	list->total_length += node->string.length;
	return node;
}
function String8Node* str8_list_push_node_set_string(String8List *list, String8Node *node, String8 string)
{
	node->string = string;
	str8_list_push_node(list, node);
	return node;
}
function String8Node* str8_list_push_node_front(String8List *list, String8Node *node)
{
	SLLQueuePushFront(list->first, list->last, node);
	list->node_count   += 1;
	list->total_length += node->string.length;
	return node;
}
function String8Node* str8_list_push_node_front_set_string(String8List *list, String8Node *node, String8 string)
{
	node->string = string;
	str8_list_push_node_front(list, node);
	return node;
}
function String8Node* str8_list_push(Arena *arena, String8List *list, String8 string)
{
	String8Node *node = push_array_no_zero(arena, String8Node, 1);
	str8_list_push_node_set_string(list, node, string);
	return node;
}
function String8Node* str8_list_push_front(Arena *arena, String8List *list, String8 string)
{
	String8Node *node = push_array_no_zero(arena, String8Node, 1);
	node->string = string;
	str8_list_push_node_front(list, node);
	return node;
}
function void str8_list_concat_in_place(String8List *list, String8List *to_push)
{
	if (to_push->node_count > 0)
	{
		if (list->last != 0)
		{
			list->total_length += to_push->total_length;
			list->node_count   += to_push->node_count;
			list->last->next    =  to_push->first;
			list->last          =  to_push->last;
		}
		else
		{
			*list = *to_push;
		}
		MemoryZeroStruct(to_push);
	}
}
function String8Node* str8_list_pushf(Arena *arena, String8List *list, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	String8 string = push_str8fv(arena, fmt, args);
	String8Node *result = str8_list_push(arena, list, string);
	va_end(args);
	return result;
}
function String8Node* str8_list_push_frontf(Arena *arena, String8List *list, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	String8 string = push_str8fv(arena, fmt, args);
	String8Node *result = str8_list_push_front(arena, list, string);
	va_end(args);
	return result;
}
function String8List str8_list_copy(Arena *arena, String8List list)
{
	String8List result = zero_struct;

	for (String8Node *node = list.first; node != 0; node = node->next)
	{
		String8 copy = push_str8_copy(arena, node->string);
		str8_list_push(arena, &result, copy);
	}

	return result;
}

function String8List str8_split(Arena *arena, String8 string, U8 *split_chars, U64 split_char_count, StringSplitFlags flags)
{
	String8List result = zero_struct;

	B32 keep_empties = flags & StringSplitFlag_KeepEmpties;

	U8 *one_past_last_string = string.buffer + string.length,
	   *one_past_last_splits = split_chars + split_char_count;
	for (U8 *c = string.buffer; c < one_past_last_string; c++)
	{
		U8 *first = c;

		for (; c < one_past_last_string; c++)
		{
			for (U8 *s = split_chars; s < one_past_last_splits; s++)
				if (*c == *s) goto break_both_inner_loops;
		}
		break_both_inner_loops:;

		String8 to_push = str8_region(first, c);
		if (to_push.length > 0 || keep_empties)
			str8_list_push(arena, &result, to_push);
	}
	return result;
}
function String8List str8_split_by_string_chars(Arena *arena, String8 string, String8 split_chars, StringSplitFlags flags)
{
	String8List result = str8_split(arena, string, split_chars.buffer, split_chars.length, flags);
	return result;
}
function String8List str8_list_split(Arena *arena, String8List list, U8 *split_chars, U64 split_char_count, StringSplitFlags flags)
{
	String8List result = {0};
	for (String8Node *node = list.first; node != 0; node = node->next)
	{
		String8List this_split = str8_split(arena, node->string, split_chars, split_char_count, flags);
		str8_list_concat_in_place(&result, &this_split);
	}
	return result;
}
function String8List str8_list_split_by_string_chars(Arena *arena, String8List list, String8 split_chars, StringSplitFlags flags)
{
	String8List result = str8_list_split(arena, list, split_chars.buffer, split_chars.length, flags);
	return result;
}
function String8 str8_list_join(Arena *arena, String8List list, StringJoin *optional_params)
{
	StringJoin params = zero_struct;
	if (optional_params != 0) params = *optional_params;

	U64 sep_count = 0;
	if (list.node_count > 0) sep_count = list.node_count - 1;
	U64 total_length = list.total_length
			   + params.pre.length
			   + params.post.length
			   + params.sep.length * sep_count;

	String8 result = push_str8(arena, total_length);
	U8 *copy_target = result.buffer;

	MemoryCopy(copy_target, params.pre.buffer, params.pre.length);
	copy_target += params.pre.length;

	for (String8Node *node = list.first; node != 0; node = node->next)
	{
		MemoryCopy(copy_target, node->string.buffer, node->string.length);
		copy_target += node->string.length;

		if (node->next != 0)
		{
			MemoryCopy(copy_target, params.sep.buffer, params.sep.length);
			copy_target += params.sep.length;
		}
	}

	MemoryCopy(copy_target, params.post.buffer, params.post.length);
	copy_target += params.post.length;

	return result;
}

function String8Array str8_array_from_list(Arena *arena, String8List list)
{
	String8Array result = {
		push_array_no_zero(arena, String8, list.node_count),
		list.node_count
	};

	String8 *copy_target = result.v;
	for (String8Node *node; node != 0; node = node->next, copy_target++)
	{
		*copy_target = node->string;
	}
	return result;
}

function String8 indented_from_string(Arena *arena, String8 string)
{
	Temp scratch = scratch_begin(&arena, 1);
	read_only local_persist U8 indentation_bytes[] = "                                                                                                                                ";
	String8List indented_strings = zero_struct;
	S64 depth = 0, next_depth = 0;
	U64 line_begin_offset = 0;
	for (U64 offset = 0; offset <= string.length; offset++)
	{
		U8 byte = offset < string.length ? string.buffer[offset] : 0;
		switch (byte)
		{
			default:{} break;
			case '{': case '[': case '(':{next_depth = ClampBot(0, next_depth + 1);} break;
			case '}': case ']': case ')':{next_depth = ClampBot(0, next_depth - 1); depth = next_depth;} break;
			case '\n':
			case 0:
			{
				String8 line = str8_trim_whitespace(str8_substr(string, r1u64(line_begin_offset, offset)));
				if (line.length > 0)
					str8_list_pushf(scratch.arena, &indented_strings, "%.*s%S\n", (int)depth*2, indentation_bytes, line);
				if (line.length == 0 && indented_strings.node_count != 0 && offset < string.length)
					str8_list_pushf(scratch.arena, &indented_strings, "\n");

				line_begin_offset = offset + 1;
				depth = next_depth;
			} break;
		}
	}
	scratch_end(scratch);
	String8 result = str8_list_join(arena, indented_strings, 0);
	return result;
}
