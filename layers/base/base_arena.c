// TODO
//  large pages
//  XXX: panic on allocation failure
//  REVIEW: move alignment stuff into os_reserve and things i.e. call AlignPow2
//   inside os_reserve and friends

internal Arena *
arena_alloc(U64 min_reserve_size, U64 min_commit_size)
{
	Assert(min_commit_size <= min_reserve_size);
	OS_SystemInfo info = os_get_system_info();

	U64 reserve = AlignPow2(min_reserve_size, info.page_size),
	    commit  = AlignPow2(min_commit_size, info.page_size);
	void *base  = os_vmem_reserve(reserve);
	B32 status  = os_vmem_commit(base, commit);
	Assert(status);

	AsanPoisonMemoryRegion(
		(U8 *)base + ARENA_HEADER_SIZE,
		reserve - ARENA_HEADER_SIZE
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
internal void *arena_push(Arena *arena, U64 size, U64 alignment)
{
	AssertAlways(size <= arena->reserved);
	U64 basepos = AlignPow2(arena->pos, alignment);
	AssertAlways(basepos <= arena->reserved);
	U64 endpos = basepos + size;
	AssertAlways(endpos <= arena->reserved);

	if (endpos > arena->committed) {
		OS_SystemInfo info = os_get_system_info();

		U8 *commit_start  = arena->base + arena->committed;
		U64 new_commitpos = AlignPow2(endpos, info.page_size),
		    commit_size   = new_commitpos - arena->committed;
		B32 status        = os_vmem_commit(commit_start, commit_size);
		Assert(status);
		AsanPoisonMemoryRegion(commit_start, commit_size);
		arena->committed = new_commitpos;
	}

	arena->pos = endpos;

	void * result = arena->base + basepos;
	AsanUnpoisonMemoryRegion(result, size);

	return result;
}

internal void arena_pop_to(Arena *arena, U64 pos)
{
	AssertAlways(pos <= arena->pos);
	U64 clampedpos = ClampBot(ARENA_HEADER_SIZE, pos); // arena's memory is in the header
	AsanPoisonMemoryRegion(arena->base + clampedpos, arena->pos - clampedpos);
	arena->pos = clampedpos;
}

internal void arena_clear(Arena *arena)
{
	arena_pop_to(arena, 0);
}

internal void arena_pop(Arena *arena, U64 amount)
{
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
