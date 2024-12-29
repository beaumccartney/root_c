#if BUILD_ENTRY_DEFINING_UNIT
function void main_thread_base_entry_point(int argc, char *argv[])
{
	set_thread_name(str8_lit("[main thread]"));
	Temp scratch = scratch_begin(0, 0);

	cmdline_global = cmd_line_from_argcv(scratch.arena, argc, argv);

	entry_point();
	scratch_end(scratch);
}
#endif

function void thread_base_entry_point(void (*entry_point)(void *params), void *params)
{
	TCTX tctx;
	tctx_init_and_equip(&tctx);
	entry_point(params);
	tctx_release();
}
