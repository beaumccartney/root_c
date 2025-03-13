// TODO
//  large pages
//  XXX: panic on allocation failure
//  REVIEW: move alignment stuff into os_reserve and things i.e. call AlignPow2
//   inside os_reserve and friends

internal Arena *
arena_alloc(S64 min_reserve_size, S64 min_commit_size)
{
	Assert(0 <= min_reserve_size && 0 <= min_commit_size && min_commit_size <= min_reserve_size && g_os_state.system_info.page_size > 0);

	S64 reserve = AlignPow2(min_reserve_size, g_os_state.system_info.page_size),
	    commit  = AlignPow2(min_commit_size, g_os_state.system_info.page_size);
	void *base  = os_vmem_reserve(reserve);
	B32 status  = os_vmem_commit(base, commit);
	Assert(status);

	AsanPoisonMemoryRegion(
		(U8 *)base + ARENA_HEADER_SIZE,
		(size_t)(reserve - ARENA_HEADER_SIZE)
	);

	Arena *arena     = (Arena *)base;
	arena->base      = base;
	arena->reserved  = reserve;
	arena->committed = commit;
	arena->pos       = ARENA_HEADER_SIZE;

	return base;
}

internal void arena_release(Arena * arena)
{
	os_vmem_release(arena->base, arena->reserved);
}

// REVIEW(beau): off-by-one bugs?
internal void *arena_push(Arena *arena, S64 size, S64 alignment)
{
	AssertAlways(0 <= alignment && 0 <= size && size <= arena->reserved);
	S64 basepos = AlignPow2(arena->pos, alignment);
	AssertAlways(basepos <= arena->reserved);
	S64 endpos = basepos + size;
	AssertAlways(endpos <= arena->reserved);

	if (endpos > arena->committed)
	{
		U8 *commit_start  = arena->base + arena->committed;
		S64 new_commitpos = AlignPow2(endpos, g_os_state.system_info.page_size),
		    commit_size   = new_commitpos - arena->committed;
		B32 status        = os_vmem_commit(commit_start, commit_size);
		Assert(status);
		AsanPoisonMemoryRegion(commit_start, (size_t)commit_size);
		arena->committed = new_commitpos;
	}

	arena->pos = endpos;

	void * result = arena->base + basepos;
	AsanUnpoisonMemoryRegion(result, (size_t)size);

	return result;
}

internal void arena_pop_to(Arena *arena, S64 pos)
{
	AssertAlways(0 <= pos && pos <= arena->pos);
	if (pos < ARENA_HEADER_SIZE)
		pos = ARENA_HEADER_SIZE;
	AsanPoisonMemoryRegion(arena->base + pos, (size_t)(arena->pos - pos));
	arena->pos = pos;
}

internal void arena_clear(Arena *arena)
{
	arena_pop_to(arena, 0);
}

internal void arena_pop(Arena *arena, S64 amount)
{
	Assert(0 <= amount);
	if (amount > arena->pos) amount = arena->pos;
	arena_pop_to(arena, arena->pos - amount);
}

internal Temp temp_begin(Arena *arena)
{
	Temp temp = {arena, arena->pos};
	return temp;
}

internal void temp_end(Temp temp)
{
	arena_pop_to(temp.arena, temp.pos);
}
