#ifndef BASE_STRINGS_H
#define BASE_STRINGS_H

typedef struct String8 String8;
struct String8
{
	U8 *str;
	U64 size;
};

#endif // BASE_STRINGS_H
