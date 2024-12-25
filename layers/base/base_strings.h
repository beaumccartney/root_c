#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

typedef struct String8 String8;
struct String8
{
	U8 *buffer;
	U64 length;
};

typedef struct String16 String16;
struct String16
{
	U16 *buffer;
	U64 length;
};

typedef struct String32 String32;
struct String32
{
	U32 *buffer;
	U64 length;
};

typedef struct String8Node String8Node;
struct String8Node
{
	String8Node *next;
	String8 string;
};

typedef struct String8List String8List;
struct String8List
{
	String8Node *first, *last;
	U64 node_count, total_length;
};

typedef struct String8Array String8Array;
struct String8Array
{
	String8 *v;
	U64 count;
};

typedef U32 StringMatchFlags;
enum
{
	StringMatchFlag_CaseInsensitive = (1 << 0),
	StringMatchFlag_RightSideSloppy = (1 << 1),
};

typedef U32 StringSplitFlags;
enum
{
	StringSplitFlag_KeepEmpties = (1 << 0),
};

typedef struct StringJoin StringJoin;
struct StringJoin
{
	String8 pre;
	String8 sep;
	String8 post;
};

function B32 char_is_space(U8 c);
function B32 char_is_upper(U8 c);
function B32 char_is_lower(U8 c);
function B32 char_is_alpha(U8 c);
function B32 char_is_slash(U8 c);
function B32 char_is_digit(U8 c, U32 base);
function U8 char_to_lower(U8 c);
function U8 char_to_upper(U8 c);
function U8 char_to_forward_slash(U8 c);
function U8 char_to_back_slash(U8 c);

function U64 cstring8_length(char *c);
function U64 cstring16_length(U16 *c);
function U64 cstring32_length(U32 *c);

#define str8_lit(S) str8((U8*)S, sizeof(S) - 1)
#define str8_varg(S) (int)((S).length), ((S).buffer)

#define str8_array(S,C) str8((U8*)(S), sizeof(*(S))*(C))
#define str8_array_fixed(S) str8((U8*)(S), sizeof((S)))
#define str8_struct(S) str8((U8*)(S), sizeof(*(S)))

#define str8(str, length) ((String8){(str),(length)})
#define str8_zero str8(0, 0)

#define str16(str, length) ((String16){(str),(length)})
#define str16_zero str16(0, 0)

#define str32(str, length) ((String32){(str),(length)})
#define str32_zero str32(0, 0)

function String8 str8_cstring(char *cstr);
function String16 str16_cstring(U16 *cstr);
function String32 str32_cstring(U32 *cstr);

function String8 str8_cstring_capped(char *cstr, U64 cap);
function String16 str16_cstring_capped(U16 *cstr, U64 cap);
function String32 str32_cstring_capped(U32 *cstr, U64 cap);

function String8 str8_region(U8* first, U8 *one_past_last);
function String16 str16_region(U16* first, U16 *one_past_last);
function String32 str32_region(U32* first, U32 *one_past_last);

function String8 push_str8(Arena *arena, U64 length);
function String8 push_str8_fill_byte(Arena *arena, U64 length, U8 byte);
function String8 push_str8_copy(Arena *arena, String8 s);
function String8 push_str8_cat(Arena *arena, String8 s1, String8 s2);
function String8 push_str8fv(Arena *arena, char *fmt, va_list args);
function String8 push_str8f(Arena *arena, char *fmt, ...);

// NOTE(beau): these add a 0 byte after the end of the string
function String8 push_str8_nt(Arena *arena, U64 length);
function String8 push_str8_fill_byte_nt(Arena *arena, U64 length, U8 byte);

function String8 str8_substr(String8 string, Rng1U64 range);
function String8 str8_prefix(String8 string, U64 length);
function String8 str8_postfix(String8 string, U64 length);
function String8 str8_skip(String8 string, U64 amt);
function String8 str8_chop(String8 string, U64 amt);
function String8 str8_trim_whitespace(String8 string);

function String8 upper_from_str8(Arena *arena, String8 string);
function String8 lower_from_str8(Arena *arena, String8 string);

function B32 str8_match(String8 a, String8 b, StringMatchFlags flags);
function U64 str8_find_needle(String8 haystack, U64 start_pos, String8 needle, StringMatchFlags flags);
function B32 str8_ends_with(String8 string, String8 end, StringMatchFlags flags);

function String8Node* str8_list_push_node(String8List *list, String8Node *node);
function String8Node* str8_list_push_node_set_string(String8List *list, String8Node *node, String8 string);
function String8Node* str8_list_push_node_front(String8List *list, String8Node *node);
function String8Node* str8_list_push_node_front_set_string(String8List *list, String8Node *node, String8 string);
function String8Node* str8_list_push(Arena *arena, String8List *list, String8 string);
function String8Node* str8_list_push_front(Arena *arena, String8List *list, String8 string);
function void         str8_list_concat_in_place(String8List *list, String8List *to_push);
function String8Node* str8_list_push_aligner(Arena *arena, String8List *list, U64 min, U64 align);
function String8Node* str8_list_pushf(Arena *arena, String8List *list, char *fmt, ...);
function String8Node* str8_list_push_frontf(Arena *arena, String8List *list, char *fmt, ...);
function String8List  str8_list_copy(Arena *arena, String8List list);

#define str8_list_first(list) ((list)->first ? (list)->first->string : str8_zero())

function String8List str8_split(Arena *arena, String8 string, U8 *split_chars, U64 split_char_count, StringSplitFlags flags);
function String8List str8_split_by_string_chars(Arena *arena, String8 string, String8 split_chars, StringSplitFlags flags);
function String8List str8_list_split(Arena *arena, String8List list, U8 *split_chars, U64 split_char_count, StringSplitFlags flags);
function String8List str8_list_split_by_string_chars(Arena *arena, String8List list, String8 split_chars, StringSplitFlags flags);
function String8     str8_list_join(Arena *arena, String8List list, StringJoin *optional_params);

function String8Array str8_array_from_list(Arena *arena, String8List list);

function String8 indented_from_string(Arena *arena, String8 string);

#endif // BASE_STRINGS_H
