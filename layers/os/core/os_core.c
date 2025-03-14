internal B32 os_handle_match(OS_Handle a, OS_Handle b)
{
	B32 result = a.bits == b.bits;
	return result;
}

internal String8 os_data_from_file_path(Arena *arena, String8 path)
{
	Assert(0 <= path.length);
	OS_Handle file = os_file_open(OS_AccessFlag_Read, path);
	FileProperties props = os_properties_from_file(file);
	Rng1S64 range = r1s64(0, props.size);
	String8 result = os_string_from_file_range(arena, file, range);
	os_file_close(file);
	return result;
}
internal B32 os_write_data_to_file_path(String8 path, String8 data)
{
	Assert(0 <= path.length);
	OS_Handle file = os_file_open(OS_AccessFlag_Write, path);
	Rng1S64 range = r1s64(0, data.length);
	os_file_write(file, range, data.buffer);
	B32 result = !os_handle_match(file, os_handle_zero);
	os_file_close(file);
	return result;
}
internal B32 os_write_data_list_to_file_path(String8 path, String8List list)
{
	Assert(0 <= list.node_count && 0 <= list.total_length);
	OS_Handle file = os_file_open(OS_AccessFlag_Write, path);
	B32 result = !os_handle_match(file, os_handle_zero);
	if (result)
	{
		S64 offset = 0;
		for (String8Node *node = list.first; node != 0; offset += node->string.length, node = node->next)
		{
			Rng1S64 range = r1s64(offset, offset + node->string.length);
			os_file_write(file, range, node->string.buffer);
		}
		os_file_close(file);
	}
	return result;
}
internal B32 os_append_data_to_file_path(String8 path, String8 data)
{
	Assert(0 <= path.length && 0 <= data.length);
	B32 result = 0;
	if (data.length > 0)
	{
		OS_Handle file = os_file_open(OS_AccessFlag_Write|OS_AccessFlag_Append, path);
		FileProperties props = os_properties_from_file(file);
		Rng1S64 range = r1s64(props.size, props.size + data.length);
		os_file_write(file, range, data.buffer);
		result = !os_handle_match(file, os_handle_zero);
		os_file_close(file);
	}
	return result;
}
internal String8 os_string_from_file_range(Arena *arena, OS_Handle file, Rng1S64 range)
{
	Assert(0 <= range.min && range.min <= range.max);
	String8 result = zero_struct;
	S64 wanted_read = dim_1s64(range);
	result.buffer = push_array_no_zero(arena, U8, wanted_read + 1);
	S64 actually_read = os_file_read(file, range, result.buffer);
	result.length = actually_read;
	result.buffer[actually_read] = 0;
	arena_pop(arena, wanted_read - actually_read);
	return result;
}
