function void set_thread_name(String8 string)
{
	Temp scratch = scratch_begin(0, 0);
	String8 copy = push_str8_copy(scratch.arena, string); // for guaranteed null terminator
	ProfThreadName((char*)copy.buffer);
	tctx_set_thread_name(copy);
	scratch_end(scratch);
}
function void set_thread_namef(char *fmt, ...)
{
	Temp scratch = scratch_begin(0, 0);
	va_list args;
	va_start(args, fmt);
	String8 string = push_str8fv(scratch.arena, fmt, args);
	ProfThreadName((char*)string.buffer);
	tctx_set_thread_name(string);
	va_end(args);
	scratch_end(scratch);
}
