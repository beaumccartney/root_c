internal void main_thread_base_entry_point(String8Array cmdline_args)
{
	set_thread_name(str8_lit("[main thread]"));
	Temp scratch = scratch_begin(0, 0);

	g_cmdline = cmd_line_from_args(scratch.arena, cmdline_args);

	#ifdef OS_GFX_H
	OS_GFX_InitReceipt gfx_receipt = os_gfx_init();
	#endif

	#ifdef RENDER_H
	R_InitReceipt r_receipt = r_init(gfx_receipt);
	Unused(r_receipt);
	#endif

	entry_point();
	scratch_end(scratch);
}

internal void thread_base_entry_point(void (*entry_point)(void *params), void *params)
{
	TCTX tctx;
	tctx_init_and_equip(&tctx);
	entry_point(params);
	tctx_release();
}
