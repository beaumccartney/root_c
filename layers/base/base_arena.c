// TODO, XXX(beau): panic on allocation failure

internal Arena *
arena_alloc(U64 min_reserve_size, U64 min_commit_size, ArenaFlags flags)
{
	OS_SystemInfo info = os_get_system_info();

	U64 page_granularity                       = 0;
	void* (*reserve_proc)(U64 size)            = 0;
	B32   (*commit_proc )(void *ptr, U64 size) = 0;
	if (flags & ArenaFlag_LargePages)
	{
		page_granularity = info.large_page_size;
		reserve_proc     = os_vmem_reserve_large;
		commit_proc      = os_vmem_commit_large;
	}
	else
	{
		page_granularity = info.page_size;
		reserve_proc     = os_vmem_reserve;
		commit_proc      = os_vmem_commit;
	}
	U64 reserve = AlignPow2(min_reserve_size, page_granularity),
	    commit  = AlignPow2(min_commit_size, page_granularity);
	void *base  = reserve_proc(reserve);
	B32 status  = commit_proc(base, commit);
	Assert(status);

	AsanPoisonMemoryRegion(base, reserve);
	AsanUnpoisonMemoryRegion(base, ARENA_HEADER_SIZE);

	Arena *arena     = (Arena *)base;
	arena->base      = base;
	arena->reserved  = reserve;
	arena->committed = commit;
	arena->pos       = ARENA_HEADER_SIZE;
	arena->flags     = flags;

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

		U64 commit_granularity = 0;
		B32 (*commit_proc)(void *ptr, U64 size) = 0;
		if (arena->flags & ArenaFlag_LargePages)
		{
			commit_granularity = info.large_page_size;
			commit_proc        = os_vmem_commit_large;
		}
		else
		{
			commit_granularity = info.page_size;
			commit_proc        = os_vmem_commit;
		}
		U8 *commit_start  = arena->base + arena->committed;
		U64 new_commitpos = AlignPow2(endpos, commit_granularity),
		    commit_size   = new_commitpos - arena->committed;
		B32 status        = commit_proc(commit_start, commit_size);
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
