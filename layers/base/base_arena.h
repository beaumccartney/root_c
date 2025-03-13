/* TODO(beau):
 *  make arena from pre-allocated backing buffer
 *  adjustable commit size
 *  adjustable decommit size (default zero)
 *  large pages
 */

#ifndef BASE_ARENA_H
#define BASE_ARENA_H

#define ARENA_HEADER_SIZE 128

typedef struct Arena Arena;
struct Arena
{
	U8 *base;
	S64 reserved, committed, pos;
};

StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

typedef struct Temp Temp;
struct Temp
{
	Arena *arena;
	S64 pos;
};

global S64 g_arena_default_reserve_size = MB(64);
global S64 g_arena_default_commit_size  = KB(64);

internal Arena *arena_alloc(S64 min_reserve, S64 min_commit);
#define arena_default arena_alloc(g_arena_default_reserve_size, g_arena_default_commit_size)
internal void arena_release(Arena * arena);

internal void *arena_push(Arena *arena, S64 size, S64 alignment);
internal void  arena_pop_to(Arena *arena, S64 pos);

internal void arena_clear(Arena *arena);
internal void arena_pop(Arena *arena, S64 amount);

internal Temp temp_begin(Arena *arena);
internal void temp_end(Temp temp);

#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), (S64)sizeof(T)*(c), (align))
#define push_array_aligned(a, T, c, align) (T *)MemoryZero(push_array_no_zero_aligned(a, T, c, align), (S64)sizeof(T)*(c))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))

#endif // BASE_ARENA
