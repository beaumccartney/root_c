#if !BUILD_SUPPLEMENTARY_UNIT
	global CmdLine *cmdline_global = 0;
#endif

internal U64
cmd_line_hash_from_string(String8 string)
{
	U64 result = chibihash64(string.buffer, string.length, 0);
	return result;
}
internal CmdLineOpt*
cmd_line_opt_from_string(Arena *arena, CmdLine *cmd_line, String8 string)
{
	U64 hash = cmd_line_hash_from_string(string);
	CmdLineOpt *result = 0;
	for (U16 table_idx = (U16)hash;;)
	{
		table_idx = msi_lookup16(hash, CMD_LINE_LOG2_OPT_COUNT, table_idx);
		CmdLineOpt *opt = &cmd_line->option_table[table_idx];
		if (opt->string.length == 0) // empty slot found
		{
			// look up failed and out of memory for insert
			AssertAlways(cmd_line->opt_count < CMD_LINE_OPT_COUNT * CMD_LINE_LOAD_FACTOR);

			if (arena != 0)
			{
				result = opt;
				result->string = push_str8_copy(arena, string);
				cmd_line->options[cmd_line->opt_count] = table_idx;
				cmd_line->opt_count++;
			}
			break;
		}
		else if (str8_match(string, opt->string, 0))
		{
			result = opt;
			break;
		}
	}
	return result;
}
internal String8List
cmd_line_strings(CmdLine *cmd_line, String8 name)
{
	CmdLineOpt *opt = cmd_line_opt_from_string(0, cmd_line, name);
	String8List result = zero_struct;
	if (opt != 0) result = opt->value_strings;
	return result;
}
internal String8
cmd_line_string(CmdLine *cmd_line, String8 name)
{
	CmdLineOpt *opt = cmd_line_opt_from_string(0, cmd_line, name);
	String8 result = zero_struct;
	if (opt != 0) result = opt->value_string;
	return result;
}
internal B32
cmd_line_has_flag(CmdLine *cmd_line, String8 name)
{
	CmdLineOpt *opt = cmd_line_opt_from_string(0, cmd_line, name);
	B32 result = opt != 0;
	return result;
}
internal B32
cmd_line_has_argument(CmdLine *cmd_line, String8 name)
{
	CmdLineOpt *opt = cmd_line_opt_from_string(0, cmd_line, name);
	B32 result = opt != 0 && opt->value_strings.node_count != 0;
	return result;
}
internal CmdLine* cmd_line_from_argcv(Arena *arena, int argc, char *argv[])
{
	CmdLine *result = push_array(arena, CmdLine, 1);

	String8List argcv_list = zero_struct;
	for (int i = 0; i < argc; i++)
	{
		String8 arg = str8_cstring(argv[i]);
		str8_list_push(arena, &argcv_list, arg);
	}

	result->exe_name = argcv_list.first->string;
	String8Node *node = argcv_list.first->next;

	// parse options and arguments - next loop will parse passthrough inputs
	for (; node != 0; node = node->next)
	{
		String8 option_name = zero_struct;
		if (str8_match(str8_prefix(node->string, 2), str8_lit("--"), 0) &&
			node->string.length > 2)
		{
			option_name = str8_skip(node->string, 2);
		}
		else if (str8_match(str8_prefix(node->string, 1), str8_lit("-"), 0) &&
			node->string.length > 2)
		{
			option_name = str8_skip(node->string, 1);
		}
		else if (operating_system_from_context() == OperatingSystem_Windows &&
			str8_match(str8_prefix(node->string, 1), str8_lit("/"), 0) &&
			node->string.length > 1)
		{
			option_name = str8_skip(node->string, 1);
		}
		// when not an option, its a passthrough option. break to get to the passthrough loop
		else
		{
			// -- is passthrough, not an actual flag. don't push it to command line structure
			if (str8_match(node->string, str8_lit("--"), 0)) node = node->next;
			break;
		}

		U64 arg_signifier_position = Min(
			str8_find_needle(option_name, 0, str8_lit(":"), 0),
			str8_find_needle(option_name, 0, str8_lit("="), 0)
		);
		B32 has_arguments = arg_signifier_position < option_name.length;

		String8List option_args = zero_struct;
		if (has_arguments)
		{
			String8 option_args_portion = str8_skip(option_name, arg_signifier_position+1);
			option_name = str8_prefix(option_name, arg_signifier_position);

			U8 splits[] = { ',' };
			option_args = str8_split(arena, option_args_portion, splits, ArrayCount(splits), 0);
		}

		// insert option
		// REVIEW(beau): what if the option is already populated?
		CmdLineOpt *opt = cmd_line_opt_from_string(arena, result, option_name);
		StringJoin joinparams = {.sep = str8_lit(",")};
		opt->value_strings = option_args;
		opt->value_string = str8_list_join(arena, opt->value_strings, &joinparams);
	}
	// passthrough option loop, starts at where option loop left off
	for (; node != 0; node = node->next)
		str8_list_push(arena, &result->passthrough_inputs, node->string);

	result->argc = argcv_list.node_count;
	result->argv = push_array_no_zero(arena, char *, result->argc);
	U64 i = 0;
	for (String8Node *arg = argcv_list.first; arg != 0; arg = arg->next, i++)
		result->argv[i] = (char *)arg->string.buffer;

	return result;
}
