#if !BUILD_SUPPLEMENTARY_UNIT
thread_local TCTX* tctx_thread_local = 0;
#endif

function void tctx_init_and_equip(TCTX *tctx)
{
	MemoryZeroStruct(tctx);
	for (U64 i = 0; i < ArrayCount(tctx->arenas); i++)
	{
		tctx->arenas[i] = arena_default;
	}
	tctx_thread_local = tctx;
}

function void tctx_release(void)
{
	for (U64 i = 0; i < ArrayCount(tctx_thread_local->arenas); i++)
	{
		arena_release(tctx_thread_local->arenas[i]);
	}
}

function TCTX* tctx_get_equipped(void)
{
	return tctx_thread_local;
}

function Arena* tctx_get_scratch(Arena **conflicts, U64 count)
{
	TCTX *tctx = tctx_get_equipped();

	Arena *result = 0;
	Arena **ctx_ptr = tctx->arenas;
	for (U64 ctx_ix = 0; ctx_ix < ArrayCount(tctx->arenas); ctx_ix++, ctx_ptr++)
	{
		B8 has_conflict = 0;
		Arena **conflict_ptr = conflicts;
		for (U64 conflict_ix = 0; conflict_ix < count; conflict_ix++, conflict_ptr++)
		{
			if (*ctx_ptr == *conflict_ptr) {
				has_conflict = 1;
				break;
			}
		}

		if (!has_conflict)
		{
			result = *ctx_ptr;
			break;
		}
	}

	return result;
}
