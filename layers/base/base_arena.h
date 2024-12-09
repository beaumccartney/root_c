/* TODO(beau):
 *  large pages
 *  make arena from pre-allocated backing buffer
 *  adjustable commit size
 */

#ifndef BASE_ARENA_H
#define BASE_ARENA_H

#define ARENA_HEADER_SIZE 128

typedef struct Arena Arena;
struct Arena
{
	void *base;
	U64 reserved, committed, pos;
	B8 large_pages;
};

StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

typedef struct Temp Temp;
struct Temp
{
	Arena *arena;
	U64 pos;
};

global U64 arena_default_reserve_size = MB(64);
global U64 arena_default_commit_size  = KB(64);

function Arena *arena_alloc(U64 min_reserve, U64 min_commit, B8 large_pages);
#define arena_default arena_alloc(arena_default_reserve_size, arena_default_commit_size, 0);
function void arena_release(Arena * arena);

function void *arena_push(Arena *arena, U64 size, U64 alignment);
function void  arena_pop_to(Arena *arena, U64 pos);

function void arena_clear(Arena *arena);
function void arena_pop(Arena *arena, U64 amount);

function Temp temp_begin(Arena *arena);
function void temp_end(Temp temp);

#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align))
#define push_array_aligned(a, T, c, align) (T *)MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)*(c))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))

#endif // BASE_ARENA
