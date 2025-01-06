/* TODO(beau):
 *  make arena from pre-allocated backing buffer
 *  adjustable commit size
 */

#ifndef BASE_ARENA_H
#define BASE_ARENA_H

#define ARENA_HEADER_SIZE 128

typedef U64 ArenaFlags;
enum
{
	ArenaFlag_LargePages = (1 << 0),
};

typedef struct Arena Arena;
struct Arena
{
	U8 *base;
	U64 reserved, committed, pos;
	ArenaFlags flags;
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
global ArenaFlags arena_default_flags = 0;

internal Arena *arena_alloc(U64 min_reserve, U64 min_commit, ArenaFlags flags);
#define arena_default arena_alloc(arena_default_reserve_size, arena_default_commit_size, arena_default_flags);
internal void arena_release(Arena * arena);

internal void *arena_push(Arena *arena, U64 size, U64 alignment);
internal void  arena_pop_to(Arena *arena, U64 pos);

internal void arena_clear(Arena *arena);
internal void arena_pop(Arena *arena, U64 amount);

internal Temp temp_begin(Arena *arena);
internal void temp_end(Temp temp);

#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align))
#define push_array_aligned(a, T, c, align) (T *)MemoryZero(push_array_no_zero_aligned(a, T, c, align), sizeof(T)*(c))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))

#endif // BASE_ARENA
