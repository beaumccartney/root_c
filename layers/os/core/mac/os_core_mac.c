internal FileProperties os_mac_file_properties_from_stat(struct stat *s)
{
	FileProperties result = zero_struct;
	result.size = s->st_size;
	if (S_ISDIR(s->st_mode)) result.flags |= FilePropertyFlag_IsFolder;
	return result;
}

internal void *os_mac_thread_entry_point(void *os_mac_entity)
{
	OS_MAC_Entity *entity = (OS_MAC_Entity *)os_mac_entity;
	TCTX tctx;
	tctx_init_and_equip(&tctx);
	entity->thread.func(entity->thread.params);
	tctx_release();
	return 0;
}

internal OS_MAC_Entity *os_mac_entity_alloc(OS_MAC_EntityKind kind)
{
	int status = pthread_mutex_lock(&g_os_mac_state.entity_mutex); Assert(status == 0);
	OS_MAC_Entity *result = g_os_mac_state.entity_free;
	if (result)
		SLLStackPop(result);
	else
		result = push_array_no_zero(g_os_mac_state.entity_arena, OS_MAC_Entity, 1);
	MemoryZeroStruct(result);
	result->kind = kind;
	status = pthread_mutex_unlock(&g_os_mac_state.entity_mutex); Assert(status == 0);
	return result;
}
internal void os_mac_entity_release(OS_MAC_Entity *entity)
{
	int status = pthread_mutex_lock(&g_os_mac_state.entity_mutex); Assert(status == 0);
	Assert(status == 0);
	SLLStackPush(g_os_mac_state.entity_free, entity);
	status = pthread_mutex_unlock(&g_os_mac_state.entity_mutex); Assert(status == 0);
	Assert(status == 0);
}

internal void *os_vmem_reserve(S64 size)
{
	Assert(0 <= size);
	void * result = 0;

	AssertAlways(vm_allocate(
		mach_task_self(),
		(vm_address_t *)&result,
		(vm_size_t)size,
		VM_FLAGS_ANYWHERE
	) == KERN_SUCCESS);

	return result;
}

internal B32 os_vmem_commit(void *ptr, S64 size)
{
	Assert(0 <= size);
	return 1;
}

internal void os_vmem_decommit(void *ptr, S64 size)
{
	Assert(0 <= size);
	// no-op
}

internal void os_vmem_release(void *ptr, S64 size)
{
	Assert(0 <= size);
	AssertAlways(vm_deallocate(
		mach_task_self(),
		(vm_address_t)ptr,
		(vm_size_t)size
	) == KERN_SUCCESS);
}

no_return internal void os_abort(S32 exit_code)
{
	exit(exit_code);
}

// REVIEW(beau): file perms based on access flags
internal OS_File os_file_open(OS_AccessFlags flags, String8 path)
{
	Assert(0 <= path.length);
	Temp scratch = scratch_begin(0, 0);
	String8 path_copy = push_str8_copy(scratch.arena, path); // guarantee null termination
	int oflag = 0;
	if (flags & OS_AccessFlag_Read && flags & OS_AccessFlag_Write)
		oflag = O_RDWR;
	else if (flags & OS_AccessFlag_Read)
		oflag = O_RDONLY;
	else if (flags & OS_AccessFlag_Write)
		oflag = O_WRONLY;

	if (flags & OS_AccessFlag_Append)
		oflag |= O_APPEND;
	if (flags & (OS_AccessFlag_Write | OS_AccessFlag_Append))
		oflag |= O_CREAT;

	int fd = open((char *)path_copy.buffer, oflag, 0644);
	OS_File result = zero_struct;
	if (fd != -1) result.bits = (U64)fd;
	scratch_end(scratch);
	return result;
}
internal void os_file_close(OS_File file)
{
	if (file.bits == 0) return;
	int fd = (int)file.bits;
	close(fd);
}
internal S64 os_file_read(OS_File file, Rng1S64 rng, void *out_data)
{
	Assert(0 <= rng.min && rng.min <= rng.max);
	if (file.bits == 0) return 0;
	int fd = (int)file.bits;
	S64 read_bytes = 0,
	    needed_bytes = dim_1s64(rng);
	while (read_bytes < needed_bytes)
	{
		void *dst           = out_data + read_bytes;
		S64 start_pos       = rng.min + read_bytes,
		    remaining_bytes = needed_bytes - read_bytes;
		ssize_t read_this_time  = pread(fd, dst, (size_t)remaining_bytes, (off_t)start_pos);
		if (read_this_time <= 0) break; // REVIEW(beau): errno?
		read_bytes += (U64)read_this_time;
	}
	return read_bytes;
}
internal S64 os_file_write(OS_File file, Rng1S64 rng, void *data)
{
	Assert(0 <= rng.min && rng.min <= rng.max);
	if (file.bits == 0) return 0;
	int fd = (int)file.bits;
	S64 written_bytes = 0,
	    goal_write_bytes = dim_1s64(rng);
	while (written_bytes < goal_write_bytes)
	{
		void *src           = data + written_bytes;
		S64 start_pos       = rng.min + written_bytes,
		    remaining_bytes = goal_write_bytes - written_bytes;
		ssize_t written_this_time  = pwrite(fd, src, (size_t)remaining_bytes, (off_t)start_pos);
		if (written_this_time <= 0) break; // REVIEW(beau): errno?
		written_bytes += (U64)written_this_time;
	}
	return written_bytes;
}
internal FileProperties os_properties_from_file(OS_File file)
{
	FileProperties result = zero_struct;
	if (file.bits != 0)
	{
		int fd = (int)file.bits;
		struct stat s = zero_struct;
		int status = fstat(fd, &s);
		if (status != -1) result = os_mac_file_properties_from_stat(&s);
	}
	return result;
}
internal B32 os_delete_file_at_path(String8 path)
{
	Assert(0 <= path.length);
	Temp scratch = scratch_begin(0, 0);
	String8 copy = push_str8_copy(scratch.arena, path); // guarantee null termination
	B32 result = unlink((char *)copy.buffer) != -1;
	scratch_end(scratch);
	return result;
}
internal B32 os_remove_folder_at_path(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	String8 copy = push_str8_copy(scratch.arena, path); // guarantee null termination
	B32 result = rmdir((char *)copy.buffer) != -1;
	scratch_end(scratch);
	return result;
}
internal B32 os_copy_file_path(String8 dst, String8 src)
{
	B32 result = 0;
	Temp scratch = scratch_begin(0, 0);
	String8 src_copy = push_str8_copy(scratch.arena, src); // guarantee null termination
	struct stat s = zero_struct;
	int status = stat((char *)src_copy.buffer, &s);
	if (status != -1 && S_ISREG(s.st_mode)) // only copy files, not folders
	{
		String8 dst_copy = push_str8_copy(scratch.arena, dst); // guarantee null termination
		status = copyfile(
			(char *)src_copy.buffer,
			(char *)dst_copy.buffer,
			0,
			COPYFILE_ALL
		);
		result = status == 0;
	}
	scratch_end(scratch);
	return result;
}
internal String8 os_full_path_from_path(Arena *arena, String8 path)
{
	Temp scratch = scratch_begin(&arena, 1);
	char buffer[PATH_MAX] = {0};
	String8 path_copy = push_str8_copy(scratch.arena, path); // guarantee null termination
	String8 result = zero_struct;
	char *realpath_result = realpath((char *)path_copy.buffer, buffer);
	if (realpath_result != 0)
		result = push_str8_copy(
			arena,
			str8_cstring_capped(buffer, ArrayCount(buffer))
		);
	scratch_end(scratch);
	return result;
}
internal B32 os_file_path_exists(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	String8 path_copy = push_str8_copy(scratch.arena, path); // guarantee null termination
	struct stat s = zero_struct;
	int status = stat((char *)path_copy.buffer, &s);
	scratch_end(scratch);
	B32 result = status != -1 && S_ISREG(s.st_mode);
	return result;
}
internal B32 os_folder_path_exists(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	String8 path_copy = push_str8_copy(scratch.arena, path); // guarantee null termination
	struct stat s = zero_struct;
	int status = stat((char *)path_copy.buffer, &s);
	scratch_end(scratch);
	B32 result = status != -1 && S_ISDIR(s.st_mode);
	return result;
}
internal FileProperties os_properties_from_file_path(String8 path)
{
	FileProperties result = zero_struct;
	Temp scratch = scratch_begin(0, 0);
	String8 path_copy = push_str8_copy(scratch.arena, path); // guarantee null termination
	struct stat s = zero_struct;
	int status = stat((char *)path_copy.buffer, &s);
	if (status != -1) result = os_mac_file_properties_from_stat(&s);
	scratch_end(scratch);
	return result;
}

internal OS_FileIter *os_file_iter_begin(Arena *arena, String8 path, OS_FileIterFlags flags)
{
	Temp scratch = scratch_begin(&arena, 1);
	String8 full_path = os_full_path_from_path(scratch.arena, path); // guarantee absolute paths from fts_read()
	char *fts_fileargs[] = {(char *)full_path.buffer, 0};
	OS_FileIter *result = push_array(arena, OS_FileIter, 1);
	result->flags = flags;
	OS_MAC_FileIter *maciter = (OS_MAC_FileIter *)result->memory;
	maciter->fts = fts_open(fts_fileargs, 0, 0);
	Assert(maciter->fts != 0);
	scratch_end(scratch);
	return result;
}
internal B32 os_file_iter_next(Arena *arena, OS_FileIter *iter, OS_FileInfo *info_out)
{
	OS_MAC_FileIter *maciter = (OS_MAC_FileIter *)iter->memory;
	FTSENT *ftsent = 0;
	for (;;) // spin until candidate or end is reached
	{
		ftsent = fts_read(maciter->fts);
		if (ftsent == 0) break;
		u_short ftsinfo = ftsent->fts_info;
		if (ftsent->fts_level == FTS_ROOTLEVEL && ftsinfo != FTS_F) continue;

		B32 skip_because_file   = iter->flags & OS_FileIterFlag_SkipFiles   && ftsinfo == FTS_F,
		    skip_because_folder = iter->flags & OS_FileIterFlag_SkipFolders && ftsinfo == FTS_D,
		    skip_because_hidden = iter->flags & OS_FileIterFlag_SkipHidden  && ftsent->fts_name[0] == '.',

		    should_skip = skip_because_file
			       || skip_because_folder
			       || skip_because_hidden;

		if (!(iter->flags & OS_FileIterFlag_RecurseFolders) || skip_because_hidden)
		{
			int status = fts_set(maciter->fts, ftsent, FTS_SKIP);
			Assert(status == 0);
		}
		if (should_skip) continue;
		if (ftsent->fts_info != FTS_DP) break;
	}
	B32 result = ftsent != 0;
	if (result)
	{
		info_out->props = os_mac_file_properties_from_stat(ftsent->fts_statp);
		String8 filerelpath = str8_cstring_capped(ftsent->fts_path, ftsent->fts_pathlen);
		info_out->name = push_str8_copy(arena, filerelpath);
	}
	return result;
}
internal void os_file_iter_end(OS_FileIter *iter)
{
	OS_MAC_FileIter *maciter = (OS_MAC_FileIter *)iter->memory;
	int status = fts_close(maciter->fts);
	Assert(status == 0);
}

internal B32 os_create_folder(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	String8 path_copy = push_str8_copy(scratch.arena, path); // guarantee null termination
	int status = mkdir((char *)path_copy.buffer, 0755);
	scratch_end(scratch);
	B32 result = status != -1;
	return result;
}

internal String8 os_get_current_folder(Arena *arena)
{
	char *buf = getcwd(0, 0);
	String8 string = str8_cstring(buf),
		result = push_str8_copy(arena, string);
	free(buf);
	return result;
}

internal S64 os_now_microseconds(void)
{
	U64 nanoseconds = clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
	Assert(nanoseconds && nanoseconds <= max_S64);
	S64 result = (S64)nanoseconds / Thousand(1); // NOTE(beau): 2^63 nanoseconds is a little bit less than 300 years
	return result;
}

internal void os_set_thread_name(String8 name)
{
	Temp scratch = scratch_begin(0, 0);
	String8 name_copy = push_str8_copy(scratch.arena, name); // guarantee null terminator
	pthread_setname_np((char *)name_copy.buffer);
	scratch_end(scratch);
}

internal OS_Thread
os_thread_launch(OS_ThreadFunctionType *func, void *params)
{
	OS_MAC_Entity *entity = os_mac_entity_alloc(OS_MAC_EntityKind_Thread);
	entity->thread.func   = func;
	entity->thread.params = params;
	int status = pthread_create(
		&entity->thread.pthread_handle,
		0,
		os_mac_thread_entry_point,
		entity
	);
	if (status != 0)
	{
		os_mac_entity_release(entity);
		entity = 0;
	}
	OS_Thread result = {.bits = (U64)entity};
	return result;
}
internal B32 os_thread_join(OS_Thread handle, S64 endt_us)
{
	B32 result = 0;
	if (handle.bits != 0)
	{
		OS_MAC_Entity *entity = (OS_MAC_Entity *)handle.bits;
		Assert(entity->kind == OS_MAC_EntityKind_Thread);
		int status = pthread_join(entity->thread.pthread_handle, 0);
		os_mac_entity_release(entity);
		result = status == 0;
	}
	return result;
}
internal void os_thread_detach(OS_Thread handle)
{
	if (handle.bits == 0) return;
	OS_MAC_Entity *entity = (OS_MAC_Entity *)handle.bits;
	Assert(entity->kind == OS_MAC_EntityKind_Thread);
	int status = pthread_detach(entity->thread.pthread_handle);
	Assert(status == 0);
	os_mac_entity_release(entity);
}

internal OS_Mutex os_mutex_alloc(void)
{
	OS_MAC_Entity *entity = os_mac_entity_alloc(OS_MAC_EntityKind_Mutex);
	int status = pthread_mutex_init(&entity->pthread_mutex_handle, 0);
	if (status != 0)
	{
		os_mac_entity_release(entity);
		entity = 0;
	}
	OS_Mutex result = {.bits = (U64)entity};
	return result;
}
internal void os_mutex_release(OS_Mutex mutex)
{
	if (mutex.bits == 0) return;
	OS_MAC_Entity *entity = (OS_MAC_Entity *)mutex.bits;
	Assert(entity->kind == OS_MAC_EntityKind_Mutex);
	int status = pthread_mutex_destroy(&entity->pthread_mutex_handle);
	Assert(status == 0);
	os_mac_entity_release(entity);
}
internal void os_mutex_take(OS_Mutex mutex)
{
	if (mutex.bits == 0) return;
	OS_MAC_Entity *entity = (OS_MAC_Entity *)mutex.bits;
	Assert(entity->kind == OS_MAC_EntityKind_Mutex);
	int status = pthread_mutex_lock(&entity->pthread_mutex_handle);
	Assert(status == 0);
}
internal void os_mutex_drop(OS_Mutex mutex)
{
	if (mutex.bits == 0) return;
	OS_MAC_Entity *entity = (OS_MAC_Entity *)mutex.bits;
	Assert(entity->kind == OS_MAC_EntityKind_Mutex);
	int status = pthread_mutex_unlock(&entity->pthread_mutex_handle);
	Assert(status == 0);
}

internal OS_RWMutex os_rw_mutex_alloc(void)
{
	OS_MAC_Entity *entity = os_mac_entity_alloc(OS_MAC_EntityKind_RWMutex);
	int status = pthread_rwlock_init(&entity->pthread_rwlock_handle, 0);
	if (status != 0)
	{
		os_mac_entity_release(entity);
		entity = 0;
	}
	OS_RWMutex result = {.bits = (U64)entity};
	return result;
}
internal void os_rw_mutex_release(OS_RWMutex rw_mutex)
{
	if (rw_mutex.bits == 0) return;
	OS_MAC_Entity *entity = (OS_MAC_Entity *)rw_mutex.bits;
	Assert(entity->kind == OS_MAC_EntityKind_RWMutex);
	int status = pthread_rwlock_destroy(&entity->pthread_rwlock_handle);
	Assert(status == 0);
	os_mac_entity_release(entity);
}
internal void os_rw_mutex_take_r(OS_RWMutex rw_mutex)
{
	if (rw_mutex.bits == 0) return;
	OS_MAC_Entity *entity = (OS_MAC_Entity *)rw_mutex.bits;
	Assert(entity->kind == OS_MAC_EntityKind_RWMutex);
	int status = pthread_rwlock_rdlock(&entity->pthread_rwlock_handle);
	Assert(status == 0);
}
internal void os_rw_mutex_drop_r(OS_RWMutex rw_mutex)
{
	if (rw_mutex.bits == 0) return;
	OS_MAC_Entity *entity = (OS_MAC_Entity *)rw_mutex.bits;
	Assert(entity->kind == OS_MAC_EntityKind_RWMutex);
	int status = pthread_rwlock_unlock(&entity->pthread_rwlock_handle);
	Assert(status == 0);
}
internal void os_rw_mutex_take_w(OS_RWMutex rw_mutex)
{
	if (rw_mutex.bits == 0) return;
	OS_MAC_Entity *entity = (OS_MAC_Entity *)rw_mutex.bits;
	Assert(entity->kind == OS_MAC_EntityKind_RWMutex);
	int status = pthread_rwlock_wrlock(&entity->pthread_rwlock_handle);
	Assert(status == 0);
}
internal void os_rw_mutex_drop_w(OS_RWMutex rw_mutex)
{
	os_rw_mutex_drop_r(rw_mutex); // in pthreads both kinds of locks are dropped the same way
}

int main(int argc, char *argv[])
{
	{
		g_os_state.system_info.page_size = (S64)vm_page_size;
		g_os_state.arena = arena_default;
		g_os_state.process_info.initial_working_directory = os_get_current_folder(g_os_state.arena);
		{
			char temp = 0, *exe_buf = &temp;
			uint32_t length = 1;
			int status = _NSGetExecutablePath(exe_buf, &length);
			if (status == -1)
			{
				exe_buf = push_array_no_zero(g_os_state.arena, char, length);
				Assert(!_NSGetExecutablePath(exe_buf, &length));
			}
			String8 exe_path = str8_cstring_capped(exe_buf, length); // REVIEW: can I use length and not scan the string?
			g_os_state.process_info.exe_folder = str8_chop_last_slash(exe_path);
		}
	}

	{
		g_os_mac_state.entity_arena = arena_default;
		int status = pthread_mutex_init(&g_os_mac_state.entity_mutex, 0);
		AssertAlways(status == 0);
	}

	local_persist TCTX tctx;
	tctx_init_and_equip(&tctx);
	{
		String8Array args = {
			.count = argc,
			.v = push_array_no_zero(g_os_state.arena, String8, argc),
		};
		for (S64 i = 0; i < args.count; i++)
			args.v[i] = str8_cstring(argv[i]);
		main_thread_base_entry_point(args);
	}
	tctx_release();

	{
		int status = pthread_mutex_destroy(&g_os_mac_state.entity_mutex);
		Assert(status == 0);
		// XXX(beau): do something with leaked entities?
		arena_release(g_os_mac_state.entity_arena);
	}
	return 0;
}
