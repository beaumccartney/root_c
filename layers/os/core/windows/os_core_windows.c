#pragma push_macro("function")
#undef function
#include <windows.h>
#pragma pop_macro("function")

function OS_SystemInfo os_get_system_info(void)
{
	SYSTEM_INFO info = zero_struct;
	GetSystemInfo(&info);

	OS_SystemInfo result = {
		.page_size = info.dwPageSize,
		.large_page_size = GetLargePageMinimum(),
	};

	return result;
}

function void *os_reserve(U64 size)
{
	return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
}

function B32 os_commit(void *ptr, U64 size)
{
	return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0;
}

function void os_decommit(void *ptr, U64 size)
{
	VirtualFree(ptr, size, MEM_DECOMMIT);
}

function void os_release(void *ptr, U64 size)
{
	VirtualFree(ptr, size, MEM_RELEASE);
}

function void *os_reserve_large(U64 size)
{
	return VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT|MEM_LARGE_PAGES, PAGE_READWRITE);
}

function B32 os_commit_large(void *ptr, U64 size)
{
	return 1;
}

function void os_set_thread_name(String8 name)
{
	Temp scratch = scratch_begin(0, 0);
	String16 name16 = str16_from_8(scratch.arena, name);
	SetThreadDescription(GetCurrentThread(), name16.buffer);
	scratch_end(scratch);
}

function void windows_entry_point_caller(int args_count, char *args[])
{
	local_persist TCTX tctx;
	tctx_init_and_equip(&tctx);
	main_thread_base_entry_point(args_count, args);
	tctx_release();
}

#if OS_FEATURE_GRAPHICAL
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	windows_entry_point_caller(__argc, __argv);
	return 0;
}
#else
void main(int argc, char *argv[])
{
	windows_entry_point_caller(argc, argv);
}
#endif
