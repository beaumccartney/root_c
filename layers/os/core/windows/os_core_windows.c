internal FilePropertyFlags
os_windows_file_property_flags_from_dwFileAttributes(DWORD dwFileAttributes)
{
	FilePropertyFlags result = 0;
	if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		result |= FilePropertyFlag_IsFolder;
	return result;
}

// NOTE(beau): path16.buffer at index path16.length must be a null terminator,
// and therefore must also be valid to access i.e. the buffer needs to have one
// more than the string's length
internal String16
os_full_path_from_path_windows_inner(Arena *arena, String16 path16)
{
	DWORD guess_length = safe_cast_u32(Max(MAX_PATH, path16.length) + 1);
	WCHAR *buf = push_array_no_zero(arena, WCHAR, guess_length);
	DWORD actual_length = GetFullPathNameW(
		path16.buffer,
		guess_length,
		buf,
		0
	);
	Assert(actual_length != 0);
	DWORD actual_length_with_nt = actual_length + 1;
	if (actual_length_with_nt <= guess_length)
	{
		arena_pop(arena, (guess_length - actual_length_with_nt) * sizeof(WCHAR));
	}
	else // == means no room for the null terminator
	{
		arena_pop(arena, guess_length * sizeof(WCHAR));
		buf = push_array_no_zero(arena, WCHAR, actual_length_with_nt);
		DWORD new_actual_length = GetFullPathNameW(
			path16.buffer,
			actual_length_with_nt,
			buf,
			0
		);
		Assert(new_actual_length == actual_length_with_nt);
	}
	Assert(buf[actual_length] == 0);
	String16 full_path16 = str16(buf, actual_length);
	return full_path16;
}

internal void
os_windows_file_iter_push(OS_FileIter *iter, U64 path_length)
{
	FINDEX_SEARCH_OPS search_op = iter->flags & OS_FileIterFlag_SkipFiles
					? FindExSearchLimitToDirectories
					: FindExSearchNameMatch;
	OS_WINDOWS_FileIter *windows_iter = (OS_WINDOWS_FileIter *)iter->memory;
	OS_WINDOWS_FileIterSearchFolder *result = push_array(windows_iter->search_dirs_arena, OS_WINDOWS_FileIterSearchFolder, 1);

	result->dirname_length = path_length + 1;
	AssertAlways(result->dirname_length + 2 <= ArrayCount(windows_iter->working_path)); // room for \, *, and 0
	windows_iter->working_path[result->dirname_length-1] = L'\\';
	windows_iter->working_path[result->dirname_length  ] = L'*';
	windows_iter->working_path[result->dirname_length+1] = L'\0';

	result->search_handle = FindFirstFileExW(
		windows_iter->working_path,
		FindExInfoBasic,
		&result->find_data,
		search_op,
		0,
		0
	);
	Assert(result->search_handle != INVALID_HANDLE_VALUE);
	SLLStackPush(windows_iter->search_dirs, result);

	windows_iter->working_path[result->dirname_length] = 0;
}

internal void *os_vmem_reserve(U64 size)
{
	return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
}

internal B32 os_vmem_commit(void *ptr, U64 size)
{
	return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0;
}

internal void os_vmem_decommit(void *ptr, U64 size)
{
	VirtualFree(ptr, size, MEM_DECOMMIT);
}

internal void os_vmem_release(void *ptr, U64 size)
{
	VirtualFree(ptr, size, MEM_RELEASE);
}

no_return internal void os_abort(S32 exit_code)
{
	ExitProcess(exit_code);
}

internal OS_Handle
os_file_open(OS_AccessFlags flags, String8 path)
{
	DWORD access               = 0,
	      creation_disposition = OPEN_EXISTING;
	if (flags & OS_AccessFlag_Read)
		access |= GENERIC_READ;
	if (flags & OS_AccessFlag_Write)
	{
		access               |= GENERIC_WRITE;
		creation_disposition  = CREATE_ALWAYS;
	}
	if (flags & OS_AccessFlag_Append)
	{
		access               |= FILE_APPEND_DATA;
		creation_disposition  = OPEN_ALWAYS;
	}
	Temp scratch    = scratch_begin(0, 0);
	String16 path16 = str16_from_8(scratch.arena, path);
	HANDLE handle   = CreateFileW(
		path16.buffer,
		access,
		0, // share mode
		0, // security attributes
		creation_disposition,
		FILE_ATTRIBUTE_NORMAL,
		0 // template file
	);
	scratch_end(scratch);
	OS_Handle result = handle == INVALID_HANDLE_VALUE
		? (OS_Handle)zero_struct
		: (OS_Handle){.u[0] = (U64)handle};
	return result;
}
internal void
os_file_close(OS_Handle file)
{
	if (!os_handle_match(file, os_handle_zero))
	{
		HANDLE win_file = (HANDLE)file.u[0];
		BOOL status     = CloseHandle(win_file);
		Assert(status);
	}
}
internal U64
os_file_read(OS_Handle file, Rng1U64 rng, void *out_data)
{
	if (os_handle_match(file, os_handle_zero)) return 0;
	HANDLE win_handle = (HANDLE)file.u[0];
	U64 read_bytes = 0,
	    needed_bytes = dim_1u64(rng);
	while (read_bytes < needed_bytes)
	{
		void *dst             = (U8 *)out_data + read_bytes;
		U64 start_pos         = rng.min + read_bytes;
		DWORD bytes_to_read = u32_from_u64_saturate(needed_bytes - read_bytes),
		      read_this_time  = 0;
		OVERLAPPED overlapped = {
			.Offset     = (start_pos      ) & 0xFFFFFFFF,
			.OffsetHigh = (start_pos >> 32) & 0xFFFFFFFF,
		};
		ReadFile(
			win_handle,
			dst,
			bytes_to_read,
			&read_this_time,
			&overlapped
		);
		if (read_this_time != bytes_to_read) break; // REVIEW(beau): why
		read_bytes += (U64)read_this_time;
	}
	return read_bytes;
}
internal U64
os_file_write(OS_Handle file, Rng1U64 rng, void *data)
{
	if (os_handle_match(file, os_handle_zero)) return 0;
	HANDLE win_handle    = (HANDLE)file.u[0];
	U64 written_bytes    = 0,
	    goal_write_bytes = dim_1u64(rng);
	while (written_bytes < goal_write_bytes)
	{
		void *src               = (U8 *)data + written_bytes;
		U64 start_pos           = rng.min + written_bytes;
		DWORD bytes_to_write    = u32_from_u64_saturate(goal_write_bytes - written_bytes),
		      written_this_time = 0;
		OVERLAPPED overlapped = {
			.Offset     = (start_pos      ) & 0xFFFFFFFF,
			.OffsetHigh = (start_pos >> 32) & 0xFFFFFFFF,
		};
		WriteFile(
			win_handle,
			src,
			bytes_to_write,
			&written_this_time,
			&overlapped
		);
		if (written_this_time != bytes_to_write) break; // REVIEW(beau): why
		written_bytes += (U64)written_this_time;
	}
	return written_bytes;
}
internal FileProperties
os_properties_from_file(OS_Handle file)
{
	FileProperties result = zero_struct;
	if (!os_handle_match(file, os_handle_zero))
	{
		HANDLE win_handle = (HANDLE)file.u[0];
		BY_HANDLE_FILE_INFORMATION info = zero_struct;
		BOOL status = GetFileInformationByHandle(win_handle, &info);
		if (status) // REVIEW(beau): assert this?
		{
			result.size  = Compose64Bit(info.nFileSizeHigh, info.nFileSizeLow);
			result.flags = os_windows_file_property_flags_from_dwFileAttributes(info.dwFileAttributes);
		}
	}
	return result;
}
internal B32
os_delete_file_at_path(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	String16 path16 = str16_from_8(scratch.arena, path);
	B32 result = DeleteFileW(path16.buffer);
	scratch_end(scratch);
	return result;
}
internal B32
os_remove_folder_at_path(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	String16 path16 = str16_from_8(scratch.arena, path);
	B32 result = RemoveDirectoryW(path16.buffer);
	scratch_end(scratch);
	return result;
}
internal B32
os_copy_file_path(String8 dst, String8 src)
{
	Temp scratch = scratch_begin(0, 0);
	String16 src16 = str16_from_8(scratch.arena, src),
		 dst16 = str16_from_8(scratch.arena, dst);
	BOOL status = CopyFileW(
		src16.buffer,
		dst16.buffer,
		0
	);
	scratch_end(scratch);
	return status;
}
internal String8
os_full_path_from_path(Arena *arena, String8 path)
{
	Temp scratch = scratch_begin(&arena, 1);
	String16 path16 = str16_from_8(scratch.arena, path);
	String16 full_path16 = os_full_path_from_path_windows_inner(scratch.arena, path16);
	String8 full_path8 = str8_from_16(arena, full_path16);
	scratch_end(scratch);
	return full_path8;
}
internal B32
os_file_path_exists(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	String16 path16 = str16_from_8(scratch.arena, path);
	DWORD attributes = GetFileAttributesW(path16.buffer);
	scratch_end(scratch);
	B32 result = attributes != INVALID_FILE_ATTRIBUTES && ~attributes & FILE_ATTRIBUTE_DIRECTORY;
	return result;
}
internal B32
os_folder_path_exists(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	String16 path16 = str16_from_8(scratch.arena, path);
	DWORD attributes = GetFileAttributesW(path16.buffer);
	scratch_end(scratch);
	B32 result = attributes != INVALID_FILE_ATTRIBUTES && attributes & FILE_ATTRIBUTE_DIRECTORY;
	return result;
}
internal FileProperties
os_properties_from_file_path(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	String16 path16 = str16_from_8(scratch.arena, path);
	WIN32_FILE_ATTRIBUTE_DATA info = zero_struct;
	BOOL status = GetFileAttributesExW(
		path16.buffer,
		GetFileExInfoStandard,
		&info
	);
	Assert(status);
	scratch_end(scratch);
	FileProperties result = {
		.size = Compose64Bit(info.nFileSizeHigh, info.nFileSizeLow),
		.flags = os_windows_file_property_flags_from_dwFileAttributes(info.dwFileAttributes)
	};
	return result;
}


internal OS_FileIter*
os_file_iter_begin(Arena *arena, String8 path, OS_FileIterFlags flags)
{
	OS_FileIter *result = 0;
	if (path.length > 0)
	{
		result = push_array(arena, OS_FileIter, 1);
		OS_WINDOWS_FileIter *windows_iter = (OS_WINDOWS_FileIter *)result->memory;
		windows_iter->search_dirs_arena = arena_default;
		result->flags = flags;

		Temp scratch = scratch_begin(&arena, 1);
		String16 relative_path16 = str16_from_8(scratch.arena, path),
			 absolute_path16 = os_full_path_from_path_windows_inner(scratch.arena, relative_path16);

		MemoryCopyTyped(
			windows_iter->working_path,
			absolute_path16.buffer,
			absolute_path16.length
		);
		os_windows_file_iter_push(result, absolute_path16.length);
		scratch_end(scratch);
	}
	return result;
}
internal B32
os_file_iter_next(Arena *arena, OS_FileIter *iter, OS_FileInfo *info_out)
{
	OS_WINDOWS_FileIter *windows_iter = (OS_WINDOWS_FileIter *)iter->memory;
	B32 status = 0;
	String16 current_file_name16 = zero_struct;
	U64 working_path_length = 0;
	FilePropertyFlags current_file_flags = 0;
	for (;;)
	{
		while (windows_iter->search_dirs != 0)
		{
			status = FindNextFileW(
				windows_iter->search_dirs->search_handle,
				&windows_iter->search_dirs->find_data
			);
			if (status)
				goto process_file;
			B32 close_status = FindClose(windows_iter->search_dirs->search_handle);
			Assert(close_status);
			SLLStackPop(windows_iter->search_dirs);
		}
		MemoryZeroStruct(info_out);
		goto end;
		process_file:;
		current_file_name16 = str16_cstring_capped(windows_iter->search_dirs->find_data.cFileName, MAX_PATH);
		Assert(current_file_name16.length > 0);
		current_file_flags = os_windows_file_property_flags_from_dwFileAttributes(windows_iter->search_dirs->find_data.dwFileAttributes);
		B32 is_folder                  = current_file_flags & FilePropertyFlag_IsFolder,
		    skip_because_file          = iter->flags & OS_FileIterFlag_SkipFiles   && !is_folder,
		    skip_because_folder        = iter->flags & OS_FileIterFlag_SkipFolders && is_folder,
		    skip_because_dot_or_dotdot = 0,
		    skip_because_hidden        = 0;
		if (current_file_name16.buffer[0] == L'.')
		{
			skip_because_dot_or_dotdot =
				current_file_name16.length == 1 ||
			       (current_file_name16.length == 2 && current_file_name16.buffer[1] == L'.');
			skip_because_hidden = iter->flags & OS_FileIterFlag_SkipHidden;
		}
		B32 skip_recurse = skip_because_hidden || skip_because_dot_or_dotdot;
		if (!skip_recurse)
		{
			B32 should_skip    = skip_because_file || skip_because_folder,
			    should_recurse = iter->flags & OS_FileIterFlag_RecurseFolders && is_folder;
			if (!should_skip || should_recurse)
			{
				working_path_length =
					windows_iter->search_dirs->dirname_length + current_file_name16.length;
				AssertAlways(working_path_length < ArrayCount(windows_iter->working_path));
				MemoryCopyTyped(
					windows_iter->working_path + windows_iter->search_dirs->dirname_length,
					current_file_name16.buffer,
					(current_file_name16.length)
				);
				windows_iter->working_path[working_path_length] = 0;
				if (should_recurse)
					os_windows_file_iter_push(iter, working_path_length);
				if (!should_skip)
					break;
			}
		}
	}
	*info_out = (OS_FileInfo) {
		.name = str8_from_16(
			arena,
			str16(
				windows_iter->working_path,
				working_path_length
			)
		),
		.props.size = Compose64Bit(
			windows_iter->search_dirs->find_data.nFileSizeHigh,
			windows_iter->search_dirs->find_data.nFileSizeLow
		),
		.props.flags = current_file_flags,
	};
	end:;
	return status;
}
internal void
os_file_iter_end(OS_FileIter *iter)
{
	OS_WINDOWS_FileIter *windows_iter = (OS_WINDOWS_FileIter *)iter->memory;
	for (OS_WINDOWS_FileIterSearchFolder *n = windows_iter->search_dirs; n != 0; SLLStackPop(n))
	{
		BOOL status = FindClose(n->search_handle);
		Assert(status);
	}
	arena_release(windows_iter->search_dirs_arena);
}

internal B32 os_create_folder(String8 path)
{
	Temp scratch    = scratch_begin(0, 0);
	String16 path16 = str16_from_8(scratch.arena, path);
	B32 result      = CreateDirectoryW(path16.buffer, 0);
	scratch_end(scratch);
	return result;
}

internal String8 os_get_current_folder(Arena *arena)
{
	Temp scratch   = scratch_begin(&arena, 1);
	DWORD length   = GetCurrentDirectoryW(0, 0);
	U16 *buffer    = push_array_no_zero(scratch.arena, U16, length + 1);
	length         = GetCurrentDirectoryW(length + 1, (WCHAR*)buffer);
	String8 result = str8_from_16(arena, str16(buffer, length));
	scratch_end(scratch);
	return result;
}

internal U64 os_elapsed_microseconds()
{
	LARGE_INTEGER perfcount;
	Assert(QueryPerformanceCounter(&perfcount));
	U64 result = (perfcount.QuadPart * 1000000) / g_os_windows_state.queryperformancefrequency_resolution;
	return result;
}

internal void os_set_thread_name(String8 name)
{
	Temp scratch = scratch_begin(0, 0);
	String16 name16 = str16_from_8(scratch.arena, name);
	SetThreadDescription(GetCurrentThread(), name16.buffer);
	scratch_end(scratch);
}

internal void windows_entry_point_caller(int argc, WCHAR *wargv[])
{
	{
		LARGE_INTEGER perffreq;
		Assert(QueryPerformanceFrequency(&perffreq));
		g_os_windows_state.queryperformancefrequency_resolution = perffreq.QuadPart;
	}
	// needed for arenas
	SYSTEM_INFO system_info = zero_struct;
	GetSystemInfo(&system_info);
	g_os_state.system_info.page_size = system_info.dwPageSize;

	local_persist TCTX tctx;
	tctx_init_and_equip(&tctx);

	g_os_state.arena = arena_alloc(MB(1), KB(32));

	g_os_state.process_info.initial_working_directory = os_get_current_folder(g_os_state.arena);
	{
		Temp scratch = scratch_begin(0, 0);

		DWORD guess = KB(16);
		LPWSTR buffer = push_array_no_zero(scratch.arena, WCHAR, guess);
		DWORD length = GetModuleFileNameW(
			0,
			buffer,
			guess
		);
		Assert(length < guess);
		String16 exe_path16 = str16(buffer, length);
		String8 exe_path = str8_from_16(g_os_state.arena, exe_path16);
		g_os_state.process_info.exe_folder = str8_chop_last_slash(exe_path);
		g_os_state.process_info.initial_working_directory = os_get_current_folder(g_os_state.arena);

		scratch_end(scratch);
	}

	char **argv = push_array_no_zero(g_os_state.arena, char*, argc);
	for (int i = 0; i < argc; i++)
	{
		String16 warg = str16_cstring(wargv[i]);
		argv[i] = (char *)str8_from_16(g_os_state.arena, warg).buffer;
	}

	main_thread_base_entry_point(argc, argv);
	tctx_release();
	arena_release(g_os_state.arena);
}

// TODO(beau): wide char versions
#if OS_FEATURE_GRAPHICAL
int WINAPI wWinMain
(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	windows_entry_point_caller(__argc, __wargv);
	return 0;
}
#else
void wmain
(int argc, wchar_t *argv[])
{
	windows_entry_point_caller(argc, argv);
}
#endif
