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

function void tctx_set_thread_name(String8 name)
{
	U8 thread_name_length = (U8)ClampTop(ArrayCount(tctx_thread_local->thread_name), name.length);
	tctx_thread_local->thread_name_length = thread_name_length;
	MemoryCopy(tctx_thread_local->thread_name, name.buffer, thread_name_length);
}
function String8 tctx_get_thread_name(void)
{
	String8 result = str8(tctx_thread_local->thread_name, tctx_thread_local->thread_name_length);
	return result;
}

function Arena* tctx_get_scratch(Arena **conflicts, U64 count)
{
	Arena *result = 0;
	Arena **ctx_ptr = tctx_thread_local->arenas;
	for (U64 ctx_ix = 0; ctx_ix < ArrayCount(tctx_thread_local->arenas); ctx_ix++, ctx_ptr++)
	{
		B32 has_conflict = 0;
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
