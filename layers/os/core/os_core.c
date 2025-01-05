internal B32 os_handle_match(OS_Handle a, OS_Handle b)
{
	B32 result = a.u[0] == b.u[0];
	return result;
}

internal String8 os_data_from_file_path(Arena *arena, String8 path)
{
	OS_Handle file = os_file_open(OS_AccessFlag_Read, path);
	FileProperties props = os_properties_from_file(file);
	Rng1U64 range = rng_1u64(0, props.size);
	String8 result = os_string_from_file_range(arena, file, range);
	os_file_close(file);
	return result;
}
internal B32 os_write_data_to_file_path(String8 path, String8 data)
{
	OS_Handle file = os_file_open(OS_AccessFlag_Write, path);
	Rng1U64 range = rng_1u64(0, data.length);
	os_file_write(file, range, data.buffer);
	B32 result = !os_handle_match(file, os_handle_zero);
	os_file_close(file);
	return result;
}
internal B32 os_write_data_list_to_file_path(String8 path, String8List list)
{
	OS_Handle file = os_file_open(OS_AccessFlag_Write, path);
	B32 result = !os_handle_match(file, os_handle_zero);
	if (result)
	{
		U64 offset = 0;
		for (String8Node *node = list.first; node != 0; offset += node->string.length, node = node->next)
		{
			Rng1U64 range = r1u64(offset, offset + node->string.length);
			os_file_write(file, range, node->string.buffer);
		}
		os_file_close(file);
	}
	return result;
}
internal B32 os_append_data_to_file_path(String8 path, String8 data)
{
	B32 result = 0;
	if (data.length > 0)
	{
		OS_Handle file = os_file_open(OS_AccessFlag_Write|OS_AccessFlag_Append, path);
		FileProperties props = os_properties_from_file(file);
		Rng1U64 range = rng_1u64(props.size, props.size + data.length);
		os_file_write(file, range, data.buffer);
		result = !os_handle_match(file, os_handle_zero);
		os_file_close(file);
	}
	return result;
}
internal String8 os_string_from_file_range(Arena *arena, OS_Handle file, Rng1U64 range)
{
	String8 result = zero_struct;
	U64 wanted_read = dim_1u64(range);
	// REVIEW(beau): null terminate the string?
	result.buffer = push_array_no_zero(arena, U8, wanted_read);
	U64 actually_read = os_file_read(file, range, result.buffer);
	result.length = actually_read;
	arena_pop(arena, wanted_read - actually_read);
	return result;
}
