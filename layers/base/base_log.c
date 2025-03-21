thread_local Log *t_log_active = 0;

internal Log *log_alloc(void)
{
	Arena * arena = arena_default();
	Log *log = push_array(arena, Log, 1);
	log->arena = arena;
	return log;
}
internal void log_release(Log *log)
{
	arena_release(log->arena);
}
internal void log_select(Log *log)
{
	t_log_active = log;
}

internal void log_msg(LogMsgKind kind, String8 string)
{
	if (t_log_active != 0 && t_log_active->top_scope != 0)
	{
		String8 copy = push_str8_copy(t_log_active->arena, string);
		str8_list_push(t_log_active->arena, &t_log_active->top_scope->strings[kind], copy);
	}
}
internal void log_msgf(LogMsgKind kind, char *fmt, ...)
{
	if (t_log_active != 0 && t_log_active->top_scope != 0)
	{
		Temp scratch = scratch_begin(0, 0);
		va_list args;
		va_start(args, fmt);
		String8 string = push_str8fv(scratch.arena, fmt, args);
		log_msg(kind, string);
		va_end(args);
		scratch_end(scratch);
	}
}

internal void log_scope_begin(void)
{
	if (t_log_active != 0)
	{
		LogScope *scope = push_array(t_log_active->arena, LogScope, 1);
		scope->log_arena_pos = t_log_active->arena->pos;
		SLLStackPush(t_log_active->top_scope, scope);
	}
}
internal LogScopeResult log_scope_end(Arena *arena)
{
	LogScopeResult result = zero_struct;
	if (t_log_active != 0)
	{
		LogScope *scope = t_log_active->top_scope;
		if (scope != 0)
		{
			SLLStackPop(t_log_active->top_scope);
			if (arena != 0)
			{
				for EachEnumVal(LogMsgKind, kind)
				{
					Temp scratch = scratch_begin(&arena, 1);
					String8 result_unindented = str8_list_join(scratch.arena, scope->strings[kind], 0);
					result.strings[kind] = indented_from_string(arena, result_unindented);
					scratch_end(scratch);
				}
			}
			arena_pop_to(t_log_active->arena, scope->log_arena_pos);
		}
	}
	return result;
}
