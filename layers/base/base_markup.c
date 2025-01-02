internal void set_thread_name(String8 name)
{
	Temp scratch = scratch_begin(0, 0);
	String8 copy = push_str8_copy(scratch.arena, name); // for guaranteed null terminator
	ProfThreadName((char*)copy.buffer);
	tctx_set_thread_name(copy);
	os_set_thread_name(copy);
	scratch_end(scratch);
}
internal void set_thread_namef(char *fmt, ...)
{
	Temp scratch = scratch_begin(0, 0);
	va_list args;
	va_start(args, fmt);
	String8 name = push_str8fv(scratch.arena, fmt, args);
	ProfThreadName((char*)name.buffer);
	tctx_set_thread_name(name);
	os_set_thread_name(name);
	va_end(args);
	scratch_end(scratch);
}
