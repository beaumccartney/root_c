internal OS_SystemInfo os_get_system_info(void)
{
	return (OS_SystemInfo){vm_page_size, MB(2)};
}

internal void *os_reserve(U64 size)
{
	void * result = 0;

	AssertAlways(vm_allocate(
		mach_task_self(),
		(vm_address_t *)&result,
		size,
		VM_FLAGS_ANYWHERE
	) == KERN_SUCCESS);

	return result;
}

internal B32 os_commit(void *ptr, U64 size)
{
	return 1;
}

internal void os_decommit(void *ptr, U64 size)
{
	// no-op
}

internal void os_release(void *ptr, U64 size)
{
	AssertAlways(vm_deallocate(
		mach_task_self(),
		(vm_address_t)ptr,
		size
	) == KERN_SUCCESS);
}

internal void *os_reserve_large(U64 size)
{
	void * result = 0;

	AssertAlways(vm_allocate(
		mach_task_self(),
		(vm_address_t *)&result,
		size,
		VM_FLAGS_ANYWHERE | VM_FLAGS_SUPERPAGE_SIZE_2MB
	) == KERN_SUCCESS);

	return result;
}

internal B32 os_commit_large(void *ptr, U64 size)
{
	return 1;
}

internal void os_abort(S32 exit_code)
{
	exit(exit_code);
}

internal void os_set_thread_name(String8 name)
{
	// TODO(beau)
}

// REVIEW(beau): file perms based on access flags
internal OS_Handle os_file_open(OS_AccessFlags flags, String8 path)
{
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
	OS_Handle result = {0};
	if (fd != -1) result.u[0] = (U64)fd;
	scratch_end(scratch);
	return result;
}
internal void os_file_close(OS_Handle file)
{
	if (os_handle_match(file, os_handle_zero)) return;
	int fd = (int)file.u[0];
	close(fd);
}
internal U64 os_file_read(OS_Handle file, Rng1U64 rng, void *out_data)
{
	if (os_handle_match(file, os_handle_zero)) return 0;
	int fd = (int)file.u[0];
	U64 read_bytes = 0,
	    needed_bytes = dim_1u64(rng);
	while (read_bytes < needed_bytes)
	{
		void *dst           = out_data + read_bytes;
		U64 start_pos       = rng.min + read_bytes,
		    remaining_bytes = needed_bytes - read_bytes;
		ssize_t read_this_time  = pread(fd, dst, remaining_bytes, (off_t)start_pos);
		if (read_this_time <= 0) break; // REVIEW(beau): errno?
		read_bytes += (U64)read_this_time;
	}
	return read_bytes;
}
internal U64 os_file_write(OS_Handle file, Rng1U64 rng, void *data)
{
	if (os_handle_match(file, os_handle_zero)) return 0;
	int fd = (int)file.u[0];
	U64 written_bytes = 0,
	    goal_write_bytes = dim_1u64(rng);
	while (written_bytes < goal_write_bytes)
	{
		void *src           = data + written_bytes;
		U64 start_pos       = rng.min + written_bytes,
		    remaining_bytes = goal_write_bytes - written_bytes;
		ssize_t written_this_time  = pwrite(fd, src, remaining_bytes, (off_t)start_pos);
		if (written_this_time <= 0) break; // REVIEW(beau): errno?
		written_bytes += (U64)written_this_time;
	}
	return written_bytes;
}
internal B32 os_delete_file_at_path(String8 path)
{
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
internal String8 os_full_path_from_path(Arena *arena, String8 path)
{
	Temp scratch = scratch_begin(&arena, 1);
	char buffer[PATH_MAX] = {0};
	String8 path_copy = push_str8_copy(scratch.arena, path); // guarantee null termination
	String8 result = zero_struct;
	if (realpath((char *)path_copy.buffer, buffer) != 0)
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
int main(int argc, char *argv[])
{
	local_persist TCTX tctx;
	tctx_init_and_equip(&tctx);
	main_thread_base_entry_point(argc, argv);
	tctx_release();
	return 0;
}
