#if BUILD_ENTRY_DEFINING_UNIT
internal void main_thread_base_entry_point(int argc, char *argv[])
{
	set_thread_name(str8_lit("[main thread]"));
	Temp scratch = scratch_begin(0, 0);

	cmdline_global = cmd_line_from_argcv(scratch.arena, argc, argv);

	#ifdef OS_GFX_H
	os_gfx_init();
	#endif

	#ifdef RENDER_CORE_H
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
