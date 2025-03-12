#if BUILD_ENTRY_DEFINING_UNIT
internal void main_thread_base_entry_point(String8Array cmdline_args)
{
	set_thread_name(str8_lit("[main thread]"));
	Temp scratch = scratch_begin(0, 0);

	g_cmdline = cmd_line_from_args(scratch.arena, cmdline_args);

	#ifdef OS_GFX_H
	os_gfx_init();
	#endif

	#ifdef RENDER_H
	r_init();
	#endif

	entry_point();
	scratch_end(scratch);
}
#endif

internal void thread_base_entry_point(void (*entry_point)(void *params), void *params)
{
	TCTX tctx;
	tctx_init_and_equip(&tctx);
	entry_point(params);
	tctx_release();
}
